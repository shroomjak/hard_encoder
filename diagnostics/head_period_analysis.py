#!/usr/bin/env python3
"""Анализ периода всех ang_<ID> и их кругового среднего.

python diagnostics/head_period_analysis.py run_01 --no-show
Вход: diagnostics/run_01/run_01.csv. Результаты — в той же папке.
Алгоритм: переходы через нуль, unwrap, МНК-тренд, общая сетка периодов,
средний остаток, СКО и ошибка среднего. seq должен строго возрастать.
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

DIAGNOSTICS = Path(__file__).resolve().parent
WRAP_DIFF_DEG = 180.0
MIN_SAMPLES = 4
ZERO_RESULTANT = 1e-9


@dataclass
class PeriodProfile:
    period_samples: float
    n_periods: int
    n_points: int
    slope_deg_per_sample: float
    intercept_deg: float
    direction: str
    sample_position: np.ndarray
    phase_deg: np.ndarray
    mean_deg: np.ndarray
    std_deg: np.ndarray
    stderr_deg: np.ndarray
    period_rows: np.ndarray


def experiment_csv(name: str) -> Path:
    if not name or '/' in name or '\\' in name or name in ('.', '..'):
        raise ValueError('Укажите имя папки внутри diagnostics')
    return DIAGNOSTICS / name / f'{name}.csv'


def angle_columns(df: pd.DataFrame) -> list[str]:
    columns = [c for c in df.columns if re.fullmatch(r'ang_\d+', str(c), re.I)]
    return sorted(columns, key=lambda c: int(str(c).split('_')[-1]))


def load_log(path: Path) -> tuple[pd.DataFrame, list[str]]:
    df = pd.read_csv(path, encoding='utf-8-sig')
    df.columns = [str(c).strip() for c in df.columns]
    columns = angle_columns(df)
    if 'seq' not in df.columns or not columns:
        raise ValueError('CSV должен содержать seq и ang_<ID>')
    return df, columns


def circular_mean_deg(matrix: np.ndarray) -> np.ndarray:
    vector = np.mean(np.exp(1j * np.deg2rad(matrix)), axis=1)
    angle = np.rad2deg(np.angle(vector)) % 360.0
    angle = np.where(angle >= 360.0, 0.0, angle)
    return np.where(np.abs(vector) >= ZERO_RESULTANT, angle, np.nan)


def build_signals(df: pd.DataFrame, columns: list[str],
                  offsets: np.ndarray | None = None) -> dict[str, tuple[np.ndarray, np.ndarray]]:
    seq = pd.to_numeric(df['seq'], errors='coerce').to_numpy(dtype=float)
    matrix = df[columns].apply(lambda c: pd.to_numeric(c, errors='coerce')).to_numpy(dtype=float)
    signals = {}
    for j, name in enumerate(columns):
        ok = np.isfinite(seq) & np.isfinite(matrix[:, j])
        signals[name] = (seq[ok], matrix[ok, j])
    rows = (np.isfinite(matrix).all(axis=1)
            & ((matrix >= 0) & (matrix < 360)).all(axis=1))
    centered = matrix if offsets is None else matrix - offsets
    mean_angle = np.full(len(seq), np.nan)
    if rows.any():
        mean_angle[rows] = circular_mean_deg(centered[rows])
    ok = np.isfinite(seq) & np.isfinite(mean_angle)
    signals['circular_mean'] = (seq[ok], mean_angle[ok])
    return signals


def unwrap_signal(angle: np.ndarray) -> np.ndarray:
    return np.rad2deg(np.unwrap(np.deg2rad(angle)))


def find_zero_transitions(seq: np.ndarray, angle: np.ndarray,
                          unwrapped: np.ndarray) -> np.ndarray:
    raw_diff = np.diff(angle)
    wrap_idx = np.flatnonzero(np.abs(raw_diff) > WRAP_DIFF_DEG)
    if len(np.unique(np.sign(raw_diff[wrap_idx]))) > 1:
        raise ValueError('Переходы через нуль в обе стороны: колебания, а не вращение')
    crossings = []
    for i in wrap_idx:
        y0, y1 = unwrapped[i], unwrapped[i + 1]
        level = round((y0 + y1) / 2.0 / 360.0) * 360.0
        if not (min(y0, y1) - 1e-9 <= level <= max(y0, y1) + 1e-9):
            raise ValueError(f'Скачок {raw_diff[i]:.1f}° без пересечения уровня 360°')
        fraction = (level - y0) / (y1 - y0)
        crossings.append(seq[i] + fraction * (seq[i + 1] - seq[i]))
    return np.asarray(crossings, dtype=float)


def analyze(seq: np.ndarray, angle: np.ndarray) -> PeriodProfile:
    seq = np.asarray(seq, dtype=float)
    angle = np.asarray(angle, dtype=float)
    if len(seq) < MIN_SAMPLES:
        raise ValueError(f'Нужно не менее {MIN_SAMPLES} отсчётов')
    if np.any(np.diff(seq) <= 0):
        raise ValueError('seq должен строго возрастать (переполнение/перезапуск/повтор)')
    if np.any((angle < 0) | (angle >= 360)):
        raise ValueError('Углы должны находиться в [0, 360)')
    unwrapped = unwrap_signal(angle)
    crossings = find_zero_transitions(seq, angle, unwrapped)
    n_periods = len(crossings) - 1
    if n_periods < 1:
        raise ValueError('Нужно минимум два перехода через нуль (один полный период)')
    period = (crossings[-1] - crossings[0]) / n_periods
    fit_mask = (seq >= crossings[0]) & (seq <= crossings[-1])
    slope, intercept = np.polyfit(seq[fit_mask], unwrapped[fit_mask], 1)
    n_points = max(2, int(round(period)))
    sample_position = np.arange(n_points) * period / n_points
    grid = crossings[0] + np.arange(n_periods)[:, None] * period + sample_position[None, :]
    period_rows = np.interp(grid, seq, unwrapped) - (slope * grid + intercept)
    mean = period_rows.mean(axis=0)
    if n_periods > 1:
        std = period_rows.std(axis=0, ddof=1)
        stderr = std / np.sqrt(n_periods)
    else:
        std = np.full(n_points, np.nan)
        stderr = np.full(n_points, np.nan)
    return PeriodProfile(
        period_samples=float(period), n_periods=n_periods, n_points=n_points,
        slope_deg_per_sample=float(slope), intercept_deg=float(intercept),
        direction='угол растёт' if slope > 0 else 'угол убывает',
        sample_position=sample_position, phase_deg=sample_position / period * 360.0,
        mean_deg=mean, std_deg=std, stderr_deg=stderr, period_rows=period_rows)


def save_csv(profile: PeriodProfile, path: Path) -> None:
    with path.open('w', encoding='utf-8', newline='') as file:
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


def build_argparser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('folder', help='Имя папки эксперимента внутри diagnostics')
    parser.add_argument('--offsets', help='Смещения перед круговым средним, по возрастанию ID: 0,180')
    parser.add_argument('--no-plots', action='store_true', help='Только CSV, без графиков')
    parser.add_argument('--no-show', action='store_true', help='Не открывать окна графиков')
    return parser


def run(args) -> None:
    path = experiment_csv(args.folder)
    df, columns = load_log(path)
    offsets = None
    if args.offsets is not None:
        offsets = np.array([float(v) for v in args.offsets.split(',')])
        if len(offsets) != len(columns) or not np.isfinite(offsets).all():
            raise ValueError('Нужно одно конечное смещение на каждый угловой столбец')
    signals = build_signals(df, columns, offsets)
    for name, (seq, angle) in signals.items():
        try:
            profile = analyze(seq, angle)
        except ValueError as exc:
            raise ValueError(f'{name}: {exc}') from None
        stem = f'{path.stem}_{name}_profile'
        save_csv(profile, path.with_name(stem + '.csv'))
        print(f'{name}: период {profile.period_samples:.6g} отсчётов; '
              f'полных периодов: {profile.n_periods}')
        if not args.no_plots:
            figure = plot_result(profile, name, path.with_name(stem + '.png'))
            if args.no_show:
                import matplotlib.pyplot as plt
                plt.close(figure)
    if not args.no_show and not args.no_plots:
        import matplotlib.pyplot as plt
        plt.show()


def main() -> None:
    args = build_argparser().parse_args()
    try:
        run(args)
    except (OSError, ValueError, pd.errors.ParserError) as exc:
        print(f'Error: {exc}', file=sys.stderr)
        raise SystemExit(1)


if __name__ == '__main__':
    main()
