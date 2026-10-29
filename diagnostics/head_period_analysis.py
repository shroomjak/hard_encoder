#!/usr/bin/env python3
"""Пооборотная статистика углов, среднего EDA и полуразности головок в единой шкале угла [0, 360°).
Вход: имя эксперимента, папка или CSV. По умолчанию тренд строится по seq; --x t_us задаёт время.
Все сигналы нарезаются по единым границам оборотов первой головки и откладываются по углу [0, 360°).
"""
from __future__ import annotations
import argparse
from dataclasses import dataclass
from itertools import combinations
from pathlib import Path
import re
import sys
import numpy as np
import pandas as pd


def resolve_csv(value):
    """Experiment name, directory, or CSV; independent of working directory."""
    base = Path(__file__).resolve().parent
    text = str(value).strip().strip('"')
    if not text:
        raise ValueError('Не указано имя эксперимента или CSV')
    p = Path(text.replace('\\', '/')).expanduser()
    if p.is_absolute():
        candidates = [p]
    elif len(p.parts) == 1:
        stem = p.stem if p.suffix.lower() == '.csv' else p.name
        candidates = [base / p, base / stem / (stem + '.csv'), base / (stem + '.csv'), p]
    else:
        candidates = [p, base / p, base.parent / p]
    tried = []
    for candidate in candidates:
        if candidate.is_dir():
            candidate = candidate / (candidate.name + '.csv')
        elif not candidate.suffix:
            expected = candidate / (candidate.name + '.csv')
            if expected.is_file():
                candidate = expected
        if candidate.is_file():
            return candidate.resolve()
        tried.append(str(candidate))
    raise FileNotFoundError('CSV не найден. Проверены пути:\n  ' + '\n  '.join(dict.fromkeys(tried)))


@dataclass
class SignalData:
    x: np.ndarray
    unwrapped: np.ndarray
    detrend: bool = True
    title: str = ''


@dataclass
class PeriodProfile:
    period_samples: float
    n_periods: int
    slope: float
    x_center: float
    y_center: float
    crossings: np.ndarray
    lengths: np.ndarray
    angle_deg: np.ndarray
    rows: np.ndarray
    mean: np.ndarray
    std: np.ndarray
    count: np.ndarray
    native_rows: list
    period_means: np.ndarray
    x_label: str = 'Угол оборота (по первой головке), град'
    title: str = ''

    @property
    def relative_x(self) -> np.ndarray:
        return self.angle_deg


def angle_columns(df):
    return sorted([c for c in df if re.fullmatch(r'ang_\d+', str(c), re.I)],
                  key=lambda name: int(name.split('_')[-1]))


def unwrap_signal(angle):
    return np.unwrap(angle, period=360.0)


def find_zero_transitions(x, angle, unwrapped):
    delta = np.diff(angle)
    indices = np.flatnonzero(np.abs(delta) > 180.0)
    if len(indices) == 0:
        return np.asarray([], dtype=float), 1
    signs = np.unique(np.sign(delta[indices]))
    if len(signs) > 1:
        raise ValueError('Переходы через нуль в обе стороны: разделите запись')
    direction = 1 if signs[0] < 0 else -1
    crossings = []
    for i in indices:
        y0, y1 = unwrapped[i:i + 2]
        level = round((y0 + y1) / 720.0) * 360.0
        if not min(y0, y1) - 1e-8 <= level <= max(y0, y1) + 1e-8:
            raise ValueError('Скачок угла без пересечения границы оборота')
        f = (level - y0) / (y1 - y0)
        crossings.append(x[i] + f * (x[i + 1] - x[i]))
    return np.asarray(crossings, dtype=float), direction


def build_reference_grid(x_ref, ang_ref, grid_step=None):
    x_ref = np.asarray(x_ref, dtype=float)
    ang_ref = np.asarray(ang_ref, dtype=float)
    ok = np.isfinite(x_ref) & np.isfinite(ang_ref)
    x_ref, ang_ref = x_ref[ok], ang_ref[ok]
    if len(x_ref) < 4:
        raise ValueError('Нужно минимум четыре конечных отсчёта опорной головки')
    dx_all = np.diff(x_ref)
    if np.any(dx_all <= 0):
        raise ValueError('Ось x должна строго возрастать; при переполнении seq используйте --x t_us или разделите файл')
    if np.any((ang_ref < 0.0) | (ang_ref >= 360.0)):
        raise ValueError('Угол должен находиться в [0, 360)')

    y_ref = unwrap_signal(ang_ref)
    crossings, direction = find_zero_transitions(x_ref, ang_ref, y_ref)
    if len(crossings) < 2:
        raise ValueError('Нужно минимум два перехода через нуль опорной головки')

    fit = (x_ref >= crossings[0]) & (x_ref <= crossings[-1])
    if fit.sum() < 2:
        raise ValueError('Недостаточно точек внутри полных оборотов')
    xc = float(0.5 * (crossings[0] + crossings[-1]))
    lengths = np.diff(crossings)
    ref_slope = float(direction * 360.0 * len(lengths) / (crossings[-1] - crossings[0]))
    if grid_step is not None:
        step_deg = float(grid_step)
        if not np.isfinite(step_deg) or step_deg <= 0.0 or step_deg > 180.0:
            raise ValueError('Шаг угловой сетки --grid-step должен находиться в (0, 180]')
        angle_grid = np.arange(0.0, 360.0, step_deg, dtype=float)
    else:
        n_bins = max(2, int(round(float(np.median(lengths)) / float(np.median(dx_all)))))
        angle_grid = np.arange(n_bins, dtype=float) * (360.0 / n_bins)

    if len(angle_grid) < 2:
        raise ValueError('Шаг сетки слишком велик для оборота')
    return crossings, direction, lengths, angle_grid, xc, ref_slope


def analyze_aligned(sig: SignalData, crossings: np.ndarray, direction: int,
                    angle_grid: np.ndarray, ref_xc: float, ref_slope: float,
                    x_label: str) -> PeriodProfile:
    xs = np.asarray(sig.x, dtype=float)
    ys = np.asarray(sig.unwrapped, dtype=float)
    ok = np.isfinite(xs) & np.isfinite(ys)
    xs, ys = xs[ok], ys[ok]
    if len(xs) < 4:
        raise ValueError('Нужно минимум четыре конечных отсчёта')
    if np.any(np.diff(xs) <= 0):
        raise ValueError('Ось x должна строго возрастать')

    fit = (xs >= crossings[0]) & (xs <= crossings[-1])
    if fit.sum() < 2:
        raise ValueError('Недостаточно точек внутри полных оборотов')

    yc = float(ys[fit].mean())
    if sig.detrend:
        slope = ref_slope
        residual = ys - (yc + slope * (xs - ref_xc))
    else:
        slope = 0.0
        residual = ys.copy()

    lengths = np.diff(crossings)
    n_periods = len(lengths)
    rows = np.full((n_periods, len(angle_grid)), np.nan, dtype=float)
    period_means = np.full(n_periods, np.nan, dtype=float)
    native = []
    frac = angle_grid / 360.0

    for k, (start, stop) in enumerate(zip(crossings[:-1], crossings[1:])):
        span = stop - start
        if direction >= 0:
            use = (xs >= start) & (xs < stop)
            x_grid = start + frac * span
            ang_native = 360.0 * (xs[use] - start) / span
        else:
            use = (xs > start) & (xs <= stop)
            x_grid = stop - frac * span
            ang_native = 360.0 * (stop - xs[use]) / span

        if use.sum() < 2:
            native.append((np.empty(0, dtype=float), np.empty(0, dtype=float), np.empty(0, dtype=float)))
            continue

        vals = np.interp(x_grid, xs, residual)
        p_mean = float(vals.mean())
        period_means[k] = p_mean
        rows[k] = vals - p_mean

        order = np.argsort(ang_native)
        native.append((ang_native[order], xs[use][order], residual[use][order] - p_mean))

    count = np.isfinite(rows).sum(axis=0)
    if np.all(count == 0):
        raise ValueError('Нет полных оборотов с валидными данными')
    mean = np.divide(np.nansum(rows, axis=0), count,
                     out=np.full(len(angle_grid), np.nan), where=count > 0)
    std = np.full(len(angle_grid), np.nan, dtype=float)
    valid_std = count > 1
    if np.any(valid_std):
        diff_sq = np.where(np.isfinite(rows), (rows - mean) ** 2, 0.0)
        std[valid_std] = np.sqrt(np.sum(diff_sq[:, valid_std], axis=0) / (count[valid_std] - 1))

    return PeriodProfile(float(lengths.mean()), n_periods, float(slope), float(ref_xc), yc,
                         crossings, lengths, angle_grid, rows, mean, std, count, native,
                         period_means, x_label=x_label, title=sig.title)


def analyze(x, angle, grid_step=None):
    """Анализ одиночного свёрнутого сигнала [0, 360) по собственным переходам через нуль."""
    x = np.asarray(x, dtype=float)
    angle = np.asarray(angle, dtype=float)
    ok = np.isfinite(x) & np.isfinite(angle)
    x_ok, ang_ok = x[ok], angle[ok]
    crossings, direction, _, angle_grid, xc, ref_slope = build_reference_grid(x_ok, ang_ok, grid_step)
    sig = SignalData(x=x_ok, unwrapped=unwrap_signal(ang_ok), detrend=True)
    return analyze_aligned(sig, crossings, direction, angle_grid, xc, ref_slope,
                           x_label='Угол оборота, град')


def load_log(path):
    df = pd.read_csv(path, encoding='utf-8-sig', sep=None, engine='python')
    df.columns = [str(c).strip() for c in df.columns]
    columns = angle_columns(df)
    if not columns:
        raise ValueError('В CSV нет столбцов ang_ID')
    return df, columns


def build_signals(df, columns, x_column='seq'):
    if x_column not in df:
        raise ValueError(f'В CSV нет столбца {x_column}')
    x = pd.to_numeric(df[x_column], errors='coerce').to_numpy(dtype=float, copy=True)
    finite = np.isfinite(x)
    if not finite.any():
        raise ValueError(f'В {x_column} нет числовых значений')
    if x_column == 'seq':
        vals = x[finite]
        if len(vals) > 1 and np.any(np.diff(vals) < -32768.0):
            x[finite] = np.unwrap(vals, period=65536.0)
    elif x_column.endswith('_us'):
        x = (x - x[finite][0]) / 1e6
    elif x_column.endswith('_ms'):
        x = (x - x[finite][0]) / 1e3

    matrix = df[columns].apply(pd.to_numeric, errors='coerce').to_numpy(dtype=float, copy=True)
    for j, name in enumerate(columns):
        flag = 'valid_' + name.split('_')[-1]
        if flag in df:
            valid = pd.to_numeric(df[flag], errors='coerce').to_numpy() == 1
            matrix[~valid, j] = np.nan
    matrix[np.isfinite(matrix) & ((matrix < 0.0) | (matrix >= 360.0))] = np.nan

    ok0 = finite & np.isfinite(matrix[:, 0])
    if ok0.sum() < 4:
        raise ValueError(f'В опорном столбце {columns[0]} недостаточно валидных отсчётов')
    u0_full = np.full(len(x), np.nan, dtype=float)
    u0_full[ok0] = unwrap_signal(matrix[ok0, 0])

    signals: dict[str, SignalData] = {}
    m = len(columns)
    ok_all = finite & np.isfinite(matrix).all(axis=1)

    if m >= 2 and ok_all.sum() >= 4:
        # Приводим начальные разности к [0, 360) и вычитаем геометрический шаг равного деления
        # 360° * j / M: это исключает переброс ветви на 180° при старте в разных полуоборотах,
        # сохраняет привязку к нулю первой головки и удерживает фазу на пропусках.
        nom = np.arange(m, dtype=float) * (360.0 / m)
        rel = np.mod(matrix[ok_all] - matrix[ok_all, :1], 360.0)
        aligned_diffs = np.unwrap(rel, axis=0, period=360.0) - nom

        for j, name in enumerate(columns):
            signals[name] = SignalData(
                x=x[ok_all],
                unwrapped=u0_full[ok_all] + aligned_diffs[:, j],
                detrend=True,
                title=name,
            )

        signals['circular_mean'] = SignalData(
            x=x[ok_all],
            unwrapped=u0_full[ok_all] + aligned_diffs.mean(axis=1),
            detrend=True,
            title='circular_mean (среднее EDA)',
        )

        if m == 2:
            signals['half_diff'] = SignalData(
                x=x[ok_all],
                unwrapped=0.5 * aligned_diffs[:, 1],
                detrend=False,
                title=f'half_diff = ({columns[1]} − {columns[0]}) / 2',
            )
        else:
            for i, j in combinations(range(m), 2):
                id_i = columns[i].split('_')[-1]
                id_j = columns[j].split('_')[-1]
                key = f'half_diff_{id_j}_{id_i}'
                signals[key] = SignalData(
                    x=x[ok_all],
                    unwrapped=0.5 * (aligned_diffs[:, j] - aligned_diffs[:, i]),
                    detrend=False,
                    title=f'({columns[j]} − {columns[i]}) / 2',
                )
    else:
        for j, name in enumerate(columns):
            ok_j = finite & np.isfinite(matrix[:, j])
            if ok_j.sum() >= 4:
                signals[name] = SignalData(
                    x=x[ok_j],
                    unwrapped=unwrap_signal(matrix[ok_j, j]),
                    detrend=True,
                    title=name,
                )

    return x[ok0], matrix[ok0, 0], signals


def save_profile(p: PeriodProfile, stem: Path):
    stderr = np.divide(p.std, np.sqrt(p.count), out=np.full(len(p.count), np.nan), where=p.count > 1)
    pd.DataFrame({'angle_deg': p.angle_deg, 'mean_deg': p.mean, 'std_deg': p.std,
                  'stderr_deg': stderr, 'count': p.count}).to_csv(str(stem) + '.csv', index=False)
    native, summary = [], []
    for k, (ang_r, xa, r) in enumerate(p.native_rows):
        native.append(pd.DataFrame({'period': k, 'angle_deg': ang_r, 'absolute_x': xa, 'residual_deg': r}))
        summary.append({'period': k, 'start_x': p.crossings[k], 'stop_x': p.crossings[k + 1],
                        'duration_x': p.lengths[k], 'native_samples': len(r),
                        'residual_mean_deg': float(p.period_means[k]),
                        'global_slope': p.slope, 'trend_x_center': p.x_center, 'trend_y_center': p.y_center})
    pd.concat(native, ignore_index=True).to_csv(str(stem) + '_native.csv', index=False)
    pd.DataFrame(summary).to_csv(str(stem) + '_periods.csv', index=False)
    pd.DataFrame({'angle_deg': p.angle_deg, **{f'period_{k}': r for k, r in enumerate(p.rows)}}).to_csv(
        str(stem) + '_overlay.csv', index=False)


def plot_result(p: PeriodProfile, name: str, path: str, unit: str):
    import matplotlib.pyplot as plt
    fig, axes = plt.subplots(3, 1, figsize=(12, 10), sharex=True, constrained_layout=True)
    title_label = p.title or name
    p2p_deg = float(np.nanmax(p.mean) - np.nanmin(p.mean))
    std_avg = float(np.nanmean(p.std))

    axes[0].plot(p.angle_deg, p.mean, 'k', label='Среднее')
    axes[0].fill_between(p.angle_deg, p.mean - p.std, p.mean + p.std, alpha=.25, label='±СКО')
    axes[0].axhline(0, color='black', lw=.5, alpha=.5)
    axes[0].set_title(
        f'{title_label}: период={p.period_samples:.6g} {unit}; оборотов={p.n_periods}; '
        f'размах={p2p_deg:.5f}° ({p2p_deg * 3600:.1f}"); '
        f'среднее СКО={std_avg:.5f}° ({std_avg * 3600:.1f}")'
    )
    axes[0].legend()

    axes[1].plot(p.angle_deg, p.std)
    axes[1].set_ylabel('СКО, град')

    for ang_r, _, residual in p.native_rows:
        if len(ang_r):
            axes[2].plot(ang_r, residual, lw=.5, alpha=.5)
    axes[2].plot(p.angle_deg, p.mean, 'k', lw=2)
    axes[2].axhline(0, color='black', lw=.5, alpha=.5)
    axes[2].set_xlim(0.0, 360.0)
    axes[2].set_xticks(np.arange(0.0, 361.0, 30.0))
    axes[2].set_xlabel(p.x_label)
    axes[0].set_ylabel('Остаток, град')
    axes[2].set_ylabel('Остаток, град')
    for ax in axes:
        ax.grid(alpha=.3)
    fig.savefig(path, dpi=150)
    return fig


def build_argparser():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('csv', nargs='?', help='Имя эксперимента, папка или CSV')
    p.add_argument('--x', default='seq', help='seq или t_us; по умолчанию seq')
    p.add_argument('--run', type=int, help='Только для старых CSV со столбцом run')
    p.add_argument('--grid-step', type=float, help='Шаг угловой сетки в градусах на [0, 360)')
    p.add_argument('--no-plots', action='store_true')
    p.add_argument('--no-show', action='store_true')
    return p


def run(args):
    name = args.csv or input('Имя эксперимента или CSV: ').strip()
    path = resolve_csv(name)
    df, columns = load_log(path)
    if 'run' in df:
        if args.run is None and df['run'].nunique() > 1:
            raise ValueError('В файле несколько poll: укажите --run N')
        if args.run is not None:
            df = df[df['run'] == args.run]
    if df.empty:
        raise ValueError('Нет строк для анализа')

    print(f'CSV: {path}')
    x_ref, ang_ref, signals = build_signals(df, columns, args.x)
    crossings, direction, _, angle_grid, ref_xc, ref_slope = build_reference_grid(
        x_ref, ang_ref, args.grid_step
    )
    unit = 'с' if args.x.endswith(('_us', '_ms')) else args.x
    x_label = (f'Угол оборота по {columns[0]}, град'
               if len(columns) >= 2 else 'Угол оборота, град')

    successes = 0
    for sig_name, sig in signals.items():
        try:
            p = analyze_aligned(sig, crossings, direction, angle_grid, ref_xc, ref_slope, x_label)
        except ValueError as exc:
            print(f'{sig_name}: {exc}', file=sys.stderr)
            continue
        stem = path.with_name(f'{path.stem}_{sig_name}_profile')
        save_profile(p, stem)
        p2p_deg = float(np.nanmax(p.mean) - np.nanmin(p.mean))
        std_avg = float(np.nanmean(p.std))
        print(f'{sig_name}: P={p.period_samples:.9g} {unit}; оборотов={p.n_periods}; '
              f'размах={p2p_deg:.6f}° ({p2p_deg * 3600:.2f}"); '
              f'СКО={std_avg:.6f}° ({std_avg * 3600:.2f}")')
        if not args.no_plots:
            fig = plot_result(p, sig_name, str(stem) + '.png', unit)
            if args.no_show:
                import matplotlib.pyplot as plt
                plt.close(fig)
        successes += 1
    if not successes:
        raise ValueError('Ни один сигнал не обработан')
    if not args.no_plots and not args.no_show:
        import matplotlib.pyplot as plt
        plt.show()


def main():
    args = build_argparser().parse_args()
    try:
        run(args)
    except (OSError, ValueError, pd.errors.ParserError) as exc:
        print(f'[ERROR] {exc}', file=sys.stderr)
        raise SystemExit(1)


if __name__ == '__main__':
    main()
