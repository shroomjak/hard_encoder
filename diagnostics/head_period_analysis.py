#!/usr/bin/env python3
"""Анализ группового лога энкодеров: CSV -> участки -> тренд -> периоды -> профиль.

Ось x — номер SNAP (seq), а не время в секундах.
Вычисляется остаток относительно равномерного вращения, не ошибка относительно
эталонного энкодера. Неравномерность двигателя тоже входит в этот остаток.
"""
from __future__ import annotations

import argparse
import csv
import json
import re
import sys
from dataclasses import dataclass
from pathlib import Path

import numpy as np
import pandas as pd


# ============================================================================
# 1. Настройки алгоритма
# ============================================================================
# Статистические коэффициенты: это не параметры конкретной головки.
MAD_TO_NORMAL_STD = 1.4826  # 1 / Phi^(-1)(0.75): перевод MAD в масштаб sigma
                           # при нормальном распределении; не обещает нормальность.
HUBER_THRESHOLD_SIGMA = 1.345  # Порог снижения веса больших остатков, в sigma.
                              # Обычный выбор: ~95% эффективности МНК при нормальности.

# Инженерные пороги. Их можно менять под реальные условия вращения.
MOTION_SPEED_PERCENTILE = 75   # Опорная скорость: 75-й процентиль ненулевых скоростей.
MOTION_SPEED_FRACTION = 0.05    # Движение: скорость не меньше 5% опорной.
MAX_REVERSE_FRACTION = 0.02     # Не больше 2% движущихся интервалов против направления.
MIN_SIGNAL_SAMPLES = 8         # Минимальная длина участка для запуска анализа.
MIN_MOVING_INTERVALS = 4       # Одного случайного скачка недостаточно для движения.
MIN_CYCLES_FOR_CLIPPING = 5    # Медианная отбраковка по оборотам требует выборки.

# Численные допуски и ограничения памяти, а не физические пороги датчика.
MIN_POSITIVE_SPEED = 1e-9      # град / SNAP: отсеиваем численный ноль скорости.
MIN_MOTION_SPEED = 1e-8        # Нижняя граница порога выделения движения.
MIN_ABS_SLOPE = 1e-12          # град / SNAP: защита деления 360 / slope.
FIT_TOLERANCE = 1e-10         # Допуск изменения коэффициентов в нормированной задаче.
MIN_RESIDUAL_SCALE = 1e-10     # град: ниже этого MAD не используем для перевзвешивания.
WEIGHT_DENOMINATOR_FLOOR = 1e-15  # Защита деления на нулевой остаток.
BOUNDARY_TOLERANCE = 1e-10     # Относительный допуск попадания границ в участок.
MAX_FIT_ITERATIONS = 30
MIN_PHASE_POINTS = 8
DEFAULT_MIN_PHASE_POINTS = 16
MAX_PHASE_POINTS = 4096
MAX_FOLD_ELEMENTS = 2_000_000  # Предел размера одной матрицы наложения.

# Размер счётчика протокола. Для старых CSV границу wrap распознаём эвристически.
SEQ_MODULUS = 1 << 16
SEQ_HALF_RANGE = SEQ_MODULUS // 2
SEQ_WRAP_HIGH = 3 * SEQ_MODULUS // 4
SEQ_WRAP_LOW = SEQ_MODULUS // 4


# ============================================================================
# 2. Результат и угловые операции
# ============================================================================
@dataclass
class PeriodAnalysisResult:
    # Сохраняем поля предыдущей версии для совместимости с построением графиков.
    period_samples: float
    n_periods: int
    phase_samples: np.ndarray
    phase_deg: np.ndarray
    mean_error: np.ndarray
    statistical_std: np.ndarray
    single_frame_std: np.ndarray
    total_std: np.ndarray
    error_matrix: np.ndarray
    period_mask: np.ndarray
    fitted_slope: float
    rotation_direction: str
    active_range: tuple[float, float]
    period_boundaries: np.ndarray
    standard_error: np.ndarray


def wrap_deg(angle):
    return np.mod(angle, 360.0)


def angle_diff(a, b):
    """Кратчайшая знаковая разность углов, диапазон [-180, 180)."""
    return (np.asarray(a) - np.asarray(b) + 180.0) % 360.0 - 180.0


def unwrap_deg(angle):
    return np.rad2deg(np.unwrap(np.deg2rad(angle)))


def circular_mean_deg(matrix, min_resultant=0.1):
    """Круговое среднее. При взаимной компенсации направлений возвращаем NaN."""
    matrix = np.asarray(matrix, dtype=float)
    if matrix.ndim != 2 or matrix.shape[1] == 0:
        raise ValueError('Для кругового среднего нужна непустая матрица')
    vector = np.mean(np.exp(1j * np.deg2rad(matrix)), axis=1)
    valid = np.isfinite(matrix).all(axis=1) & (np.abs(vector) >= min_resultant)
    return np.where(valid, np.rad2deg(np.angle(vector)) % 360.0, np.nan)


# ============================================================================
# 3. Подготовка одного непрерывного сигнала
# ============================================================================
def _prepare_signal(x, angle, std, max_gap, max_step):
    """Одна проверка входных массивов и отсечение неподвижных краёв.

    Пропуски/перезапуски разделяет split_signal() до вызова анализатора.
    Здесь остаётся проверка контракта: analyze_signal() можно вызвать напрямую.
    std: либо известные неотрицательные СКО, либо целиком NaN (неизвестны).
    """
    x, angle, std = [np.asarray(a, dtype=float) for a in (x, angle, std)]
    if any(a.ndim != 1 for a in (x, angle, std)):
        raise ValueError('x, angle и std должны быть одномерными')
    if not len(x) == len(angle) == len(std) or len(x) < MIN_SIGNAL_SAMPLES:
        raise ValueError(f'Нужны массивы одинаковой длины, не менее {MIN_SIGNAL_SAMPLES} точек')
    if not (np.isfinite(x).all() and np.isfinite(angle).all()):
        raise ValueError('В x/angle есть NaN или бесконечность')
    if np.any((angle < 0) | (angle >= 360)):
        raise ValueError('Углы должны находиться в [0, 360)')
    std_known = np.isfinite(std).all()
    if not (std_known or np.isnan(std).all()) or (std_known and np.any(std < 0)):
        raise ValueError('std: неотрицательные конечные значения или целиком NaN')

    dx = np.diff(x)
    delta_angle = angle_diff(angle[1:], angle[:-1])
    if np.any(dx <= 0) or np.any(dx > max_gap) or np.any(np.abs(delta_angle) > max_step):
        raise ValueError('Участок не непрерывный: повтор x, пропуск или угловой скачок')

    speed = np.abs(delta_angle / dx)
    positive_speed = speed[speed > MIN_POSITIVE_SPEED]
    if len(positive_speed) < MIN_MOVING_INTERVALS:
        raise ValueError('Нет устойчивого движения')
    reference_speed = np.percentile(positive_speed, MOTION_SPEED_PERCENTILE)
    motion_threshold = max(MIN_MOTION_SPEED, MOTION_SPEED_FRACTION * reference_speed)
    moving = np.flatnonzero(speed >= motion_threshold)
    if len(moving) < MIN_MOVING_INTERVALS:
        raise ValueError('Недостаточно движущихся интервалов')

    # Интервал i соединяет точки i и i+1; stop сдвигаем на 2 для Python slice.
    active_slice = slice(int(moving[0]), int(moving[-1]) + 2)
    return x[active_slice], angle[active_slice], std[active_slice], motion_threshold


# ============================================================================
# 4. Робастная прямая и границы полных периодов модели
# ============================================================================
def robust_line(x, y):
    """Прямая y = slope*x + intercept, итеративное МНК с весами Huber.

    Малые остатки имеют вес 1. Для больших вес уменьшается как threshold/|r|.
    Центрирование x улучшает обусловленность при больших номерах seq.
    Ни исходные углы, ни отдельные точки здесь не удаляются и не сглаживаются.
    """
    center = float(np.mean(x))
    x_span = float(np.ptp(x))
    if x_span <= 0:
        raise ValueError('Нулевой диапазон x')
    design = np.column_stack(((x - center) / x_span, np.ones(len(x))))
    coefficients = np.linalg.lstsq(design, y, rcond=None)[0]

    for _ in range(MAX_FIT_ITERATIONS):
        residual = y - design @ coefficients
        mad = np.median(np.abs(residual - np.median(residual)))
        residual_scale = MAD_TO_NORMAL_STD * mad
        if residual_scale < MIN_RESIDUAL_SCALE:
            break

        threshold = HUBER_THRESHOLD_SIGMA * residual_scale
        weights = np.minimum(1.0, threshold / np.maximum(np.abs(residual), WEIGHT_DENOMINATOR_FLOOR))
        root_weights = np.sqrt(weights)
        updated = np.linalg.lstsq(design * root_weights[:, None], y * root_weights, rcond=None)[0]
        change = np.max(np.abs(updated - coefficients))
        coefficients = updated
        if change < FIT_TOLERANCE:
            break

    slope = float(coefficients[0] / x_span)
    intercept = float(coefficients[1] - slope * center)
    return slope, intercept


def find_period_boundaries(x, slope, intercept):
    """Пересечения прямой с уровнями 360*n внутри доступного участка.

    Это границы модели равномерного вращения, не измеренные события оборота.
    Границы на концах участка сохраняем с численным допуском.
    """
    if not np.isfinite(slope) or abs(slope) < MIN_ABS_SLOPE:
        raise ValueError('Наклон близок к нулю')
    endpoints = slope * np.asarray([x[0], x[-1]]) + intercept
    first_turn = np.ceil(np.min(endpoints) / 360.0 - BOUNDARY_TOLERANCE)
    last_turn = np.floor(np.max(endpoints) / 360.0 + BOUNDARY_TOLERANCE)
    turn_numbers = np.arange(first_turn, last_turn + 1)
    boundaries = np.sort((360.0 * turn_numbers - intercept) / slope)
    tolerance = max(1.0, float(np.ptp(x))) * BOUNDARY_TOLERANCE
    inside = (boundaries >= x[0] - tolerance) & (boundaries <= x[-1] + tolerance)
    return np.clip(boundaries[inside], x[0], x[-1])


def _fold_periods(x, unwrapped, std, boundaries, slope, intercept, n_phase):
    """Одна строка матрицы — один оборот, один столбец — одна фаза.

    Интерполируем только внутри предварительно проверенного непрерывного участка.
    Узлы находятся в центрах фазовых ячеек: (i+0.5)/N.
    Для обратного вращения переставляем столбцы в порядок возрастания угловой фазы.
    """
    fraction = (np.arange(n_phase) + 0.5) / n_phase
    query_x = boundaries[:-1, None] + np.diff(boundaries)[:, None] * fraction
    measured = np.interp(query_x.ravel(), x, unwrapped).reshape(query_x.shape)
    residuals = measured - (slope * query_x + intercept)

    if np.isnan(std).all():
        instrument_std = np.full_like(residuals, np.nan)
    else:
        instrument_std = np.interp(query_x.ravel(), x, std).reshape(query_x.shape)

    phase_deg = fraction * 360.0 if slope > 0 else 360.0 - fraction * 360.0
    order = np.argsort(phase_deg)
    return fraction[order], phase_deg[order], residuals[:, order], instrument_std[:, order]


def _select_periods(residuals, clip):
    """Необязательная отбраковка целых оборотов, не отдельных точек.

    Оценка оборота — RMS отклонения от медианного профиля.
    clip=0 сохраняет все обороты; иначе порог = median(score) + clip*sigma_MAD.
    Это инженерный фильтр, а не гарантия вероятности ложного срабатывания.
    """
    keep = np.ones(len(residuals), dtype=bool)
    if clip <= 0 or len(residuals) < MIN_CYCLES_FOR_CLIPPING:
        return keep
    median_profile = np.median(residuals, axis=0)
    scores = np.sqrt(np.mean((residuals - median_profile) ** 2, axis=1))
    median_score = np.median(scores)
    score_scale = MAD_TO_NORMAL_STD * np.median(np.abs(scores - median_score))
    return scores <= median_score + clip * max(score_scale, MIN_RESIDUAL_SCALE)


# ============================================================================
# 5. Основной расчёт: только последовательность этапов
# ============================================================================
def analyze_signal(x, angle, std, args):
    """Анализ одного непрерывного участка. Настройки args проверяет validate_options()."""
    x, angle, std, motion_threshold = _prepare_signal(x, angle, std, args.max_gap, args.max_step)

    # 1. Оцениваем тренд и период по развёрнутому углу.
    unwrapped = unwrap_deg(angle)
    slope, intercept = robust_line(x, unwrapped)
    if not np.isfinite(slope) or abs(slope) < MIN_ABS_SLOPE:
        raise ValueError('Нельзя определить период: наклон близок к нулю')
    period = 360.0 / abs(slope)
    if args.min_period is not None and period < args.min_period:
        raise ValueError(f'Период {period:g} меньше --min-period')
    if args.max_period is not None and period > args.max_period:
        raise ValueError(f'Период {period:g} больше --max-period')

    # 2. Один участок должен иметь преимущественно одно направление вращения.
    signed_speed = angle_diff(angle[1:], angle[:-1]) / np.diff(x)
    moving_speed = signed_speed[np.abs(signed_speed) >= motion_threshold]
    reverse_fraction = np.mean(moving_speed * np.sign(slope) < 0)
    if reverse_fraction > MAX_REVERSE_FRACTION:
        raise ValueError('На участке есть существенный разворот; разделите измерение')

    # 3. Выбираем фазовую сетку и полные периоды, без искусственного clamp периода.
    points_per_turn = period / np.median(np.diff(x))
    if points_per_turn < args.min_points:
        raise ValueError('Недостаточно измерений на оборот')
    n_phase = args.n_phase
    if n_phase is None:
        n_phase = min(MAX_PHASE_POINTS, max(DEFAULT_MIN_PHASE_POINTS, int(round(points_per_turn))))
    boundaries = find_period_boundaries(x, slope, intercept)
    n_periods = len(boundaries) - 1
    if n_periods < 1:
        raise ValueError('Нет полного периода модели внутри участка')
    if n_periods * n_phase > MAX_FOLD_ELEMENTS:
        raise ValueError('Слишком большая матрица; уменьшите --n-phase или разделите лог')

    # 4. Накладываем обороты и, только по явному запросу, исключаем выбросы.
    fraction, phase_deg, residuals, instrument_std = _fold_periods(
        x, unwrapped, std, boundaries, slope, intercept, n_phase)
    keep = _select_periods(residuals, 0.0 if args.no_filter else args.clip)
    selected = residuals[keep]
    n_selected = len(selected)
    if n_selected == 0:
        raise ValueError('Отбракованы все обороты')

    # 5. Наблюдаемый разброс не складываем повторно с инструментальным шумом.
    mean_residual = np.mean(selected, axis=0)
    observed_std = (np.std(selected, axis=0, ddof=1) if n_selected > 1
                    else np.full(n_phase, np.nan))
    supplied_std = np.sqrt(np.mean(instrument_std[keep] ** 2, axis=0))

    return PeriodAnalysisResult(
        period_samples=float(period), n_periods=n_selected,
        phase_samples=fraction * period, phase_deg=phase_deg,
        mean_error=mean_residual, statistical_std=observed_std,
        single_frame_std=supplied_std, total_std=observed_std.copy(),
        error_matrix=selected, period_mask=keep, fitted_slope=slope,
        rotation_direction='increasing' if slope > 0 else 'decreasing',
        active_range=(float(x[0]), float(x[-1])), period_boundaries=boundaries,
        # Эта оценка ошибки среднего предполагает независимость оборотов.
        standard_error=observed_std / np.sqrt(n_selected))


# ============================================================================
# 6. CSV и разделение потока: не сортировать данные по seq
# ============================================================================
def _angle_columns(df):
    return sorted([c for c in df.columns if re.fullmatch(r'ang_\d+', c, re.I)],
                  key=lambda c: int(c.split('_')[-1]))


def prepare_log(path):
    """Сохраняем порядок строк. Для legacy CSV восстанавливаем wrap uint16.

    session надёжнее эвристики wrap: его пишет новый логгер при обнаружении reset.
    Если boot-маркер потерян, reset может быть неразличим с обычным ходом seq.
    """
    df = pd.read_csv(path, encoding='utf-8-sig', on_bad_lines='error')
    df.columns = [str(c).strip() for c in df.columns]
    if 'seq' not in df.columns or not _angle_columns(df):
        raise ValueError('CSV должен содержать seq и ang_1..ang_k')
    for column in ['seq', 'status', 'session', *_angle_columns(df)]:
        if column in df:
            df[column] = pd.to_numeric(df[column], errors='coerce')
    if 'status' not in df:
        print('[WARN] Нет status: проверяем только числовые значения', file=sys.stderr)
        df['status'] = 1

    sessions = df['session'].to_numpy() if 'session' in df else np.zeros(len(df))
    x = np.full(len(df), np.nan)
    segments = np.zeros(len(df), dtype=int)
    previous_seq, previous_session, previous_x, segment = None, None, 0.0, 0
    for i, seq in enumerate(df['seq'].to_numpy()):
        session = sessions[i]
        if not np.isfinite(seq) or seq < 0 or seq != np.floor(seq) or not np.isfinite(session):
            segment += 1
            previous_seq = None
            segments[i] = segment
            continue
        if previous_seq is None or session != previous_session:
            segment += 1
            value = float(seq)
        elif seq > previous_seq:
            value = previous_x + seq - previous_seq
        elif (SEQ_WRAP_HIGH <= previous_seq < SEQ_MODULUS and seq < SEQ_WRAP_LOW
              and 0 < (seq - previous_seq) % SEQ_MODULUS < SEQ_HALF_RANGE):
            value = previous_x + (seq - previous_seq) % SEQ_MODULUS
        else:
            segment += 1
            value = float(seq)
        x[i], segments[i] = value, segment
        previous_seq, previous_session, previous_x = seq, session, value
    df['_x'], df['_segment'] = x, segments
    return df


def split_signal(x, angle, valid, labels, max_gap=3.0, max_step=90.0):
    """Разрывы не интерполируем. Одиночный выброс может отделить два хороших участка."""
    pieces, start = [], None
    for i in range(len(x)):
        good = bool(valid[i]) and np.isfinite(x[i]) and np.isfinite(angle[i]) and 0 <= angle[i] < 360
        boundary = False
        if good and start is not None:
            dx = x[i] - x[i - 1]
            boundary = (labels[i] != labels[i - 1] or not 0 < dx <= max_gap
                        or abs(angle_diff(angle[i], angle[i - 1])) > max_step)
        if (not good or boundary) and start is not None:
            pieces.append(slice(start, i))
            start = None
        if good and start is None:
            start = i
    if start is not None:
        pieces.append(slice(start, len(x)))
    return pieces


# ============================================================================
# 7. Сохранение результатов
# ============================================================================
def save_csv(result, path):
    with open(path, 'w', encoding='utf-8', newline='') as file:
        writer = csv.writer(file)
        writer.writerow(['phase_samples', 'phase_deg', 'mean_residual_deg', 'observed_std_deg',
                         'mean_stderr_independent_turns_deg', 'supplied_instrument_std_deg'])
        writer.writerows(zip(result.phase_samples, result.phase_deg, result.mean_error,
                             result.statistical_std, result.standard_error, result.single_frame_std))


def plot_result(result, out_path=None):
    import matplotlib.pyplot as plt
    fig, axes = plt.subplots(3, 1, figsize=(12, 10), sharex=True, constrained_layout=True)
    phase = result.phase_deg
    axes[0].plot(phase, result.mean_error, 'k', label='Mean residual')
    axes[0].fill_between(phase, result.mean_error - result.statistical_std,
                         result.mean_error + result.statistical_std,
                         alpha=0.25, label='Observed inter-turn SD')
    axes[0].set_title(f'Period={result.period_samples:.5g} SNAP cycles; N={result.n_periods}; {result.rotation_direction}')
    axes[0].set_ylabel('Residual, deg')
    axes[0].legend()
    axes[1].plot(phase, result.statistical_std, label='Observed SD')
    if np.isfinite(result.single_frame_std).any():
        axes[1].plot(phase, result.single_frame_std, label='Supplied instrument SD (not added)')
    axes[1].set_ylabel('SD, deg')
    axes[1].legend()
    for row in result.error_matrix[:20]:
        axes[2].plot(phase, row, lw=0.6, alpha=0.6)
    axes[2].plot(phase, result.mean_error, 'k', lw=2)
    axes[2].set_xlabel('Model phase, deg')
    axes[2].set_ylabel('Residual, deg')
    for ax in axes:
        ax.set_xlim(0, 360)
        ax.grid(alpha=0.3)
    if out_path:
        fig.savefig(out_path, dpi=150)
    return fig


# ============================================================================
# 8. CLI: проверяем настройки один раз, до обработки сигналов
# ============================================================================
def build_argparser():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('csv')
    parser.add_argument('--min-period', type=float)
    parser.add_argument('--max-period', type=float)
    parser.add_argument('--n-phase', type=int)
    parser.add_argument('--max-gap', type=float, default=3.0, help='Максимальный интервал seq внутри участка')
    parser.add_argument('--max-step', type=float, default=90.0, help='Максимальный круговой шаг, град; меньше 180')
    parser.add_argument('--min-points', type=int, default=12, help='Минимум измерений на оборот')
    parser.add_argument('--clip', type=float, default=0.0, help='MAD-порог отбраковки оборотов; 0 = выключено')
    parser.add_argument('--no-filter', action='store_true', help='Не отбраковывать обороты')
    parser.add_argument('--n-hint', type=float, default=None, help='Устаревшая опция, выдаёт предупреждение')
    parser.add_argument('--offsets', help='Вычитаемые смещения перед круговым средним, например 0,180')
    parser.add_argument('--min-resultant', type=float, default=0.1, help='Минимальная длина среднего единичных векторов')
    parser.add_argument('--no-show', action='store_true')
    parser.add_argument('--no-plots', action='store_true', help='Только CSV и JSON, без matplotlib')
    return parser


def validate_options(args):
    """Проверка параметров вызова. В вычислительных этапах её не дублируем."""
    if not np.isfinite(args.max_gap) or args.max_gap <= 0 or not 0 < args.max_step < 180:
        raise ValueError('Нужны max-gap > 0 и 0 < max-step < 180')
    if not np.isfinite(args.clip) or args.clip < 0 or args.min_points < 4:
        raise ValueError('Некорректные clip/min-points')
    if args.n_phase is not None and not MIN_PHASE_POINTS <= args.n_phase <= MAX_PHASE_POINTS:
        raise ValueError(f'n-phase должен быть в {MIN_PHASE_POINTS}..{MAX_PHASE_POINTS}')
    if not 0 < args.min_resultant <= 1:
        raise ValueError('min-resultant должен быть в (0, 1]')
    bounds = [v for v in (args.min_period, args.max_period) if v is not None]
    if any(not np.isfinite(v) or v <= 0 for v in bounds):
        raise ValueError('Границы периода должны быть конечными и положительными')
    if len(bounds) == 2 and args.min_period > args.max_period:
        raise ValueError('min-period больше max-period')
    if args.n_hint is not None:
        print('[WARN] --n-hint устарел и не используется', file=sys.stderr)


def run(args):
    validate_options(args)
    path = Path(args.csv)
    df = prepare_log(path)
    columns = _angle_columns(df)
    offsets = (np.zeros(len(columns)) if args.offsets is None
               else np.array([float(v) for v in args.offsets.split(',')]))
    if len(offsets) != len(columns) or not np.isfinite(offsets).all():
        raise ValueError('Нужно одно конечное смещение на каждый угловой столбец')

    out_dir = path.parent / path.stem
    out_dir.mkdir(parents=True, exist_ok=True)
    x, labels = df['_x'].to_numpy(), df['_segment'].to_numpy()
    valid = df['status'].to_numpy() == 1
    matrix = df[columns].to_numpy(dtype=float)
    signals = [(column, matrix[:, i]) for i, column in enumerate(columns)]
    if len(columns) > 1:
        valid_angles = np.isfinite(matrix).all(axis=1) & ((matrix >= 0) & (matrix < 360)).all(axis=1)
        mean_angle = circular_mean_deg(wrap_deg(matrix - offsets), args.min_resultant)
        mean_angle[~valid_angles] = np.nan
        signals.append(('circular_mean', mean_angle))

    summary = []
    for name, angle in signals:
        pieces = split_signal(x, angle, valid, labels, args.max_gap, args.max_step)
        for number, piece in enumerate(pieces, 1):
            size = piece.stop - piece.start
            if size < MIN_SIGNAL_SAMPLES:
                continue
            stem = f'{name}_segment_{number:03d}'
            try:
                result = analyze_signal(x[piece], angle[piece], np.full(size, np.nan), args)
            except ValueError as exc:
                summary.append({'signal': name, 'segment': number, 'status': 'rejected', 'reason': str(exc)})
                print(f'[WARN] {stem}: {exc}', file=sys.stderr)
                continue
            save_csv(result, out_dir / f'{stem}_profile.csv')
            if not args.no_plots:
                figure = plot_result(result, out_dir / f'{stem}.png')
                if args.no_show:
                    import matplotlib.pyplot as plt
                    plt.close(figure)
            summary.append({'signal': name, 'segment': number, 'status': 'ok',
                            'period_snap_cycles': result.period_samples, 'turns': result.n_periods,
                            'rejected_turns': int((~result.period_mask).sum()),
                            'direction': result.rotation_direction, 'range': result.active_range})
            print(f'{stem}: period={result.period_samples:.6g} SNAP cycles; turns={result.n_periods}')

    (out_dir / 'analysis_summary.json').write_text(json.dumps({
        'warning': 'Residual against a constant-speed model, not true encoder error. seq is not seconds. '
                   'Hidden turns cannot be uniquely recovered. SEM assumes independent turns. '
                   'Legacy reset/wrap detection is heuristic.',
        'source': str(path), 'results': summary}, ensure_ascii=False, indent=2), encoding='utf-8')
    if not any(item['status'] == 'ok' for item in summary):
        raise ValueError('Нет пригодного участка: смотрите analysis_summary.json')
    if not args.no_show and not args.no_plots:
        import matplotlib.pyplot as plt
        plt.show()


def main():
    try:
        run(build_argparser().parse_args())
    except (OSError, ValueError, pd.errors.ParserError) as exc:
        print(f'[ERROR] {exc}', file=sys.stderr)
        raise SystemExit(1)


if __name__ == '__main__':
    main()