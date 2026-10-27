#!/usr/bin/env python3
"""Анализ периода сигналов головок энкодера из CSV-лога.

Обрабатываются все угловые столбцы ang_1..ang_k, а также круговое среднее
по всем головкам circular_mean(ang_1..ang_k) — при одной головке оно совпадает
с её сигналом. Для каждого сигнала:
0. Сигнал берётся из CSV-лога, ось x — счётчик seq.
1. По разностям соседних отсчётов находятся переходы через нуль:
   |diff| > 180° — это скачки около ±360°.
2. Берутся только полные периоды: между первым и последним таким переходом.
3. Сигнал разворачивается (unwrap), по МНК ищется наклон (град/отсчёт)
   для последующего детрендирования.
4. Период P = (последний переход - первый) / число полных периодов.
   Значения восстанавливаются линейной интерполяцией на общую сетку
   отсчётов внутри периода; пропусков на сетке нет.
5. Периоды накладываются друг на друга по модулю P: k-я строка сдвинута
   относительно первой ровно на k*P отсчётов.
6. После вычитания тренда считаются среднее и СКО в каждой точке периода.
7. Три графика на сигнал: среднее ± СКО; СКО от положения внутри периода;
   все периоды вместе со средним.

Запуск:
    python head_period_analysis_v2.py log_example.csv
"""
from __future__ import annotations

import argparse
import csv
import re
import sys
from dataclasses import dataclass
from pathlib import Path

import numpy as np
import pandas as pd

# Порог перехода через нуль: |diff| > 180° (скачок около ±360°).
# Предположение метода: реальное перемещение между соседними доступными
# отсчётами меньше 180°, иначе число оборотов между ними неоднозначно.
WRAP_DIFF_DEG = 180.0
MIN_SAMPLES = 4
# Круговое среднее не определено только при численно нулевом среднем векторе.
# Слабый, но ненулевой вектор даёт определённое, хотя и зашумлённое среднее —
# оно сохраняется, количество таких строк выводится предупреждением.
ZERO_RESULTANT = 1e-9
WEAK_RESULTANT = 0.1


@dataclass
class PeriodProfile:
    period_samples: float          # период, отсчётов seq
    n_periods: int                # число полных периодов
    n_points: int                 # точек сетки внутри периода
    slope_deg_per_sample: float   # наклон МНК, град/отсчёт
    intercept_deg: float          # смещение МНК-прямой, град
    direction: str                # 'угол растёт' / 'угол убывает'
    sample_position: np.ndarray   # положение внутри периода, отсчёты (n_points,)
    phase_deg: np.ndarray         # то же в градусах фазы, 0..360 (n_points,)
    mean_deg: np.ndarray          # среднее по периодам (n_points,)
    std_deg: np.ndarray           # СКО по периодам (n_points,)
    stderr_deg: np.ndarray        # ошибка среднего (n_points,)
    period_rows: np.ndarray      # все периоды, (n_periods, n_points)


# ============================================================================
# 0. Загрузка лога и сигналов
# ============================================================================
def angle_columns(df: pd.DataFrame) -> list[str]:
    """Столбцы ang_1..ang_k в порядке номеров."""
    columns = [c for c in df.columns if re.fullmatch(r'ang_\d+', str(c), re.I)]
    return sorted(columns, key=lambda c: int(str(c).split('_')[-1]))


def load_log(path: Path) -> tuple[pd.DataFrame, list[str]]:
    df = pd.read_csv(path, encoding='utf-8-sig')
    df.columns = [str(c).strip() for c in df.columns]
    if 'seq' not in df.columns:
        raise ValueError('CSV должен содержать столбец seq')
    columns = angle_columns(df)
    if not columns:
        raise ValueError('CSV должен содержать столбцы ang_1..ang_k')
    if 'session' in df.columns:
        session = pd.to_numeric(df['session'], errors='coerce').to_numpy(dtype=float)
        if np.unique(session[np.isfinite(session)]).size > 1:
            raise ValueError('В логе несколько сессий (перезапуск): разделите файл')
    return df, columns


def circular_mean_deg(matrix: np.ndarray) -> np.ndarray:
    """Круговое среднее углов по строкам; NaN только при численно нулевом векторе.

    Результат гарантированно в [0, 360): возможное после взятия модуля ровно
    360.0 (округление крошечного отрицательного угла) приводится к нулю.
    """
    vector = np.mean(np.exp(1j * np.deg2rad(matrix)), axis=1)
    angle = np.rad2deg(np.angle(vector)) % 360.0
    angle = np.where(angle >= 360.0, 0.0, angle)
    return np.where(np.abs(vector) >= ZERO_RESULTANT, angle, np.nan)


def build_signals(df: pd.DataFrame, columns: list[str],
                  offsets: np.ndarray | None = None) -> dict[str, tuple[np.ndarray, np.ndarray]]:
    """Сигналы: каждый столбец ang_k и circular_mean по всем головкам.

    Строки с пустым значением сигнала пропускаются, значения seq сохраняются.
    Круговое среднее строится по строкам, где все исходные углы конечны и лежат
    в [0, 360); проверка выполняется до вычитания смещений (--offsets).
    """
    seq = pd.to_numeric(df['seq'], errors='coerce').to_numpy(dtype=float)
    matrix = df[columns].apply(lambda c: pd.to_numeric(c, errors='coerce')).to_numpy(dtype=float)
    signals: dict[str, tuple[np.ndarray, np.ndarray]] = {}
    for j, name in enumerate(columns):
        ok = np.isfinite(seq) & np.isfinite(matrix[:, j])
        signals[name] = (seq[ok], matrix[ok, j])

    rows = (np.isfinite(matrix).all(axis=1)
            & ((matrix >= 0) & (matrix < 360)).all(axis=1))
    centered = matrix if offsets is None else matrix - offsets
    mean_angle = np.full(len(seq), np.nan)
    if rows.any():
        mean_angle[rows] = circular_mean_deg(centered[rows])
        vector = np.mean(np.exp(1j * np.deg2rad(centered[rows])), axis=1)
        n_weak = int((np.abs(vector) < WEAK_RESULTANT).sum())
        if n_weak:
            print(f'[WARN] circular_mean: {n_weak} строк со слабым средним вектором '
                  f'(< {WEAK_RESULTANT}): среднее существует, но зашумлено', file=sys.stderr)
    ok = np.isfinite(seq) & np.isfinite(mean_angle)
    signals['circular_mean'] = (seq[ok], mean_angle[ok])
    return signals


# ============================================================================
# 1-2. Переходы через нуль и развёрнутый сигнал
# ============================================================================
def unwrap_signal(angle: np.ndarray) -> np.ndarray:
    return np.rad2deg(np.unwrap(np.deg2rad(angle)))


def find_zero_transitions(seq: np.ndarray, angle: np.ndarray,
                          unwrapped: np.ndarray) -> np.ndarray:
    """Координаты переходов через нуль (пересечения уровней 360*n), в отсчётах seq.

    Переход обнаруживается по |diff| > 180° между соседними углами, положение
    уточняется линейной интерполяцией по развёрнутому сигналу.
    """
    raw_diff = np.diff(angle)
    wrap_idx = np.flatnonzero(np.abs(raw_diff) > WRAP_DIFF_DEG)
    if len(np.unique(np.sign(raw_diff[wrap_idx]))) > 1:
        raise ValueError('Переходы через нуль в обе стороны: колебания, а не вращение')

    crossings = []
    for i in wrap_idx:
        y0, y1 = unwrapped[i], unwrapped[i + 1]
        # Пересекаемый уровень 360*n — ближайший к середине интервала;
        # он может совпадать с концом интервала (точный нуль угла).
        level = round((y0 + y1) / 2.0 / 360.0) * 360.0
        if not (min(y0, y1) - 1e-9 <= level <= max(y0, y1) + 1e-9):
            raise ValueError(f'Скачок {raw_diff[i]:.1f}° без пересечения уровня 360°')
        fraction = (level - y0) / (y1 - y0)
        crossings.append(seq[i] + fraction * (seq[i + 1] - seq[i]))
    return np.asarray(crossings, dtype=float)


# ============================================================================
# 3-6. Основной расчёт
# ============================================================================
def analyze(seq: np.ndarray, angle: np.ndarray) -> PeriodProfile:
    seq = np.asarray(seq, dtype=float)
    angle = np.asarray(angle, dtype=float)
    if len(seq) < MIN_SAMPLES:
        raise ValueError(f'Нужно не менее {MIN_SAMPLES} отсчётов')
    if np.any(np.diff(seq) <= 0):
        raise ValueError('seq должен строго возрастать (перезапуск/повтор строк)')
    if np.any((angle < 0) | (angle >= 360)):
        raise ValueError('Углы должны находиться в [0, 360)')

    unwrapped = unwrap_signal(angle)
    crossings = find_zero_transitions(seq, angle, unwrapped)
    n_periods = len(crossings) - 1
    if n_periods < 1:
        raise ValueError('Нужно минимум два перехода через нуль (один полный период)')

    # 4. Период по переходам; тренд — одна МНК-прямая по всему диапазону.
    period = (crossings[-1] - crossings[0]) / n_periods
    fit_mask = (seq >= crossings[0]) & (seq <= crossings[-1])
    slope, intercept = np.polyfit(seq[fit_mask], unwrapped[fit_mask], 1)

    # 5. Общая сетка: k-я строка сдвинута ровно на k*P, без растяжения периодов.
    n_points = max(2, int(round(period)))
    sample_position = np.arange(n_points) * period / n_points
    grid = crossings[0] + np.arange(n_periods)[:, None] * period + sample_position[None, :]
    # Все точки сетки лежат строго внутри данных, np.interp не выходит за края.
    period_rows = np.interp(grid, seq, unwrapped) - (slope * grid + intercept)

    # 6. Среднее и СКО в каждой точке периода.
    mean = period_rows.mean(axis=0)
    if n_periods > 1:
        std = period_rows.std(axis=0, ddof=1)
        stderr = std / np.sqrt(n_periods)
    else:
        print('[WARN] Только один полный период: СКО не определено', file=sys.stderr)
        std = np.full(n_points, np.nan)
        stderr = np.full(n_points, np.nan)

    return PeriodProfile(
        period_samples=float(period), n_periods=n_periods, n_points=n_points,
        slope_deg_per_sample=float(slope), intercept_deg=float(intercept),
        direction='угол растёт' if slope > 0 else 'угол убывает',
        sample_position=sample_position, phase_deg=sample_position / period * 360.0,
        mean_deg=mean, std_deg=std, stderr_deg=stderr, period_rows=period_rows)


# ============================================================================
# 7. Сохранение и графики
# ============================================================================
def save_csv(profile: PeriodProfile, path: Path) -> None:
    with open(path, 'w', encoding='utf-8', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(['sample_position', 'phase_deg', 'mean_deg', 'std_deg', 'stderr_deg'])
        writer.writerows(zip(profile.sample_position, profile.phase_deg,
                             profile.mean_deg, profile.std_deg, profile.stderr_deg))


def plot_result(profile: PeriodProfile, name: str, out_path: Path | None = None):
    import matplotlib.pyplot as plt
    x = profile.sample_position
    fig, axes = plt.subplots(3, 1, figsize=(12, 10), sharex=True, constrained_layout=True)

    axes[0].plot(x, profile.mean_deg, 'k-', label='Среднее')
    axes[0].fill_between(x, profile.mean_deg - profile.std_deg,
                         profile.mean_deg + profile.std_deg, alpha=0.25, label='±СКО')
    axes[0].set_title(f'{name}: период = {profile.period_samples:.6g} отсчётов; '
                      f'полных периодов: {profile.n_periods}; {profile.direction}')
    axes[0].set_ylabel('Остаток, град')
    axes[0].legend()

    axes[1].plot(x, profile.std_deg)
    axes[1].set_ylabel('СКО, град')
    axes[1].set_title('СКО от положения внутри периода')

    for row in profile.period_rows:
        axes[2].plot(x, row, lw=0.4, alpha=0.4)
    axes[2].plot(x, profile.mean_deg, 'k-', lw=2, label='Среднее')
    axes[2].set_xlabel('Положение внутри периода, отсчёты')
    axes[2].set_ylabel('Остаток, град')
    axes[2].set_title('Все периоды и среднее')
    axes[2].legend()

    for ax in axes:
        ax.grid(alpha=0.3)
    if out_path:
        fig.savefig(out_path, dpi=150)
    return fig


# ============================================================================
# CLI
# ============================================================================
def build_argparser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('csv', help='CSV-файл лога (seq, ang_1..ang_k)')
    parser.add_argument('--offsets', help='Смещения головок перед круговым средним, '
                                         'например 0,180 (по одному на столбец ang)')
    parser.add_argument('--no-plots', action='store_true', help='Только CSV, без графиков')
    parser.add_argument('--no-show', action='store_true', help='Не открывать окна графиков')
    return parser


def run(args) -> None:
    path = Path(args.csv)
    df, columns = load_log(path)
    offsets = None
    if args.offsets is not None:
        offsets = np.array([float(v) for v in args.offsets.split(',')])
        if len(offsets) != len(columns) or not np.isfinite(offsets).all():
            raise ValueError('Нужно одно конечное смещение на каждый угловой столбец')

    signals = build_signals(df, columns, offsets)
    n_ok = 0
    for name, (seq, angle) in signals.items():
        stem = f'{path.stem}_{name}_profile'
        try:
            profile = analyze(seq, angle)
        except ValueError as exc:
            print(f'[WARN] {name}: {exc}', file=sys.stderr)
            continue
        save_csv(profile, path.with_name(stem + '.csv'))
        print(f'{name}: период {profile.period_samples:.6g} отсчётов; '
              f'полных периодов: {profile.n_periods}')
        if not args.no_plots:
            figure = plot_result(profile, name, path.with_name(stem + '.png'))
            if args.no_show:
                import matplotlib.pyplot as plt
                plt.close(figure)
        n_ok += 1

    if n_ok == 0:
        raise ValueError('Ни один сигнал не проанализирован: смотрите предупреждения выше')
    if not args.no_show and not args.no_plots:
        import matplotlib.pyplot as plt
        plt.show()


def main() -> None:
    args = build_argparser().parse_args()
    try:
        run(args)
    except (OSError, ValueError, pd.errors.ParserError) as exc:
        print(f'[ERROR] {exc}', file=sys.stderr)
        raise SystemExit(1)


if __name__ == '__main__':
    main()
