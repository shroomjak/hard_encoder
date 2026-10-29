#!/usr/bin/env python3
"""Глобальный тренд и статистика оборотов без подгонки отдельных периодов.
Вход: имя эксперимента, папка или CSV. По умолчанию x=seq; --x t_us задаёт время.
"""
from __future__ import annotations
import argparse
from dataclasses import dataclass
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
class PeriodProfile:
    period_samples: float
    n_periods: int
    slope: float
    x_center: float
    y_center: float
    crossings: np.ndarray
    lengths: np.ndarray
    relative_x: np.ndarray
    rows: np.ndarray
    mean: np.ndarray
    std: np.ndarray
    count: np.ndarray
    native_rows: list
    x_label: str = 'seq от начала оборота'


def angle_columns(df):
    return sorted([c for c in df if re.fullmatch(r'ang_\d+', str(c), re.I)],
                  key=lambda name: int(name.split('_')[-1]))


def unwrap_signal(angle):
    return np.rad2deg(np.unwrap(np.deg2rad(angle)))


def find_zero_transitions(x, angle, unwrapped):
    delta = np.diff(angle)
    indices = np.flatnonzero(np.abs(delta) > 180)
    if len(np.unique(np.sign(delta[indices]))) > 1:
        raise ValueError('Переходы через нуль в обе стороны: разделите запись')
    crossings = []
    for i in indices:
        y0, y1 = unwrapped[i:i + 2]
        level = round((y0 + y1) / 720) * 360
        if not min(y0, y1) - 1e-8 <= level <= max(y0, y1) + 1e-8:
            raise ValueError('Скачок угла без пересечения границы оборота')
        f = (level - y0) / (y1 - y0)
        crossings.append(x[i] + f * (x[i + 1] - x[i]))
    return np.asarray(crossings)


def analyze(x, angle, grid_step=None):
    x, angle = np.asarray(x, dtype=float), np.asarray(angle, dtype=float)
    ok = np.isfinite(x) & np.isfinite(angle)
    x, angle = x[ok], angle[ok]
    if len(x) < 4:
        raise ValueError('Нужно минимум четыре конечных отсчёта')
    if np.any(np.diff(x) <= 0):
        raise ValueError('Ось x должна строго возрастать; при переполнении seq используйте --x t_us или разделите файл')
    if np.any((angle < 0) | (angle >= 360)):
        raise ValueError('Угол должен находиться в [0,360)')
    y = unwrap_signal(angle)
    crossings = find_zero_transitions(x, angle, y)
    if len(crossings) < 2:
        raise ValueError('Нужно минимум два перехода через нуль')
    fit = (x >= crossings[0]) & (x <= crossings[-1])
    if fit.sum() < 2:
        raise ValueError('Недостаточно точек внутри полных оборотов')
    xc, yc = x[fit].mean(), y[fit].mean()
    dx = x[fit] - xc
    slope = np.dot(dx, y[fit] - yc) / np.dot(dx, dx)
    residual = y - (yc + slope * (x - xc))
    lengths = np.diff(crossings)
    step = float(grid_step) if grid_step is not None else float(np.median(np.diff(x)))
    if not np.isfinite(step) or step <= 0:
        raise ValueError('Шаг сетки должен быть положительным')
    relative = np.arange(0., lengths.max(), step)
    if len(relative) < 2:
        raise ValueError('Шаг сетки слишком велик для оборота')
    rows = np.full((len(lengths), len(relative)), np.nan)
    native = []
    for k, (start, stop) in enumerate(zip(crossings[:-1], crossings[1:])):
        supported = relative < stop - start
        rows[k, supported] = np.interp(start + relative[supported], x, residual)
        use = (x >= start) & (x < stop)
        native.append((x[use] - start, x[use], residual[use]))
    count = np.isfinite(rows).sum(axis=0)
    mean = np.divide(np.nansum(rows, axis=0), count,
                     out=np.full(len(relative), np.nan), where=count > 0)
    variance = np.divide(np.nansum((rows - mean) ** 2, axis=0), count - 1,
                         out=np.full(len(relative), np.nan), where=count > 1)
    return PeriodProfile(float(lengths.mean()), len(lengths), float(slope), float(xc), float(yc),
                         crossings, lengths, relative, rows, mean, np.sqrt(variance), count, native)


def load_log(path):
    df = pd.read_csv(path, encoding='utf-8-sig', sep=None, engine='python')
    df.columns = [str(c).strip() for c in df.columns]
    columns = angle_columns(df)
    if not columns:
        raise ValueError('В CSV нет столбцов ang_ID')
    return df, columns


def build_signals(df, columns, offsets=None, x_column='seq'):
    if x_column not in df:
        raise ValueError(f'В CSV нет столбца {x_column}')
    x = pd.to_numeric(df[x_column], errors='coerce').to_numpy(float)
    finite = np.isfinite(x)
    if not finite.any():
        raise ValueError(f'В {x_column} нет числовых значений')
    if x_column.endswith('_us'):
        x = (x - x[finite][0]) / 1e6
    elif x_column.endswith('_ms'):
        x = (x - x[finite][0]) / 1e3
    matrix = df[columns].apply(pd.to_numeric, errors='coerce').to_numpy(float)
    for j, name in enumerate(columns):
        flag = 'valid_' + name.split('_')[-1]
        if flag in df:
            valid = pd.to_numeric(df[flag], errors='coerce').to_numpy() == 1
            matrix[~valid, j] = np.nan
    signals = {name: (x, matrix[:, j]) for j, name in enumerate(columns)}
    ok = np.isfinite(matrix).all(axis=1) & ((matrix >= 0) & (matrix < 360)).all(axis=1)
    centered = matrix if offsets is None else matrix - offsets
    means = np.full(len(x), np.nan)
    if ok.any():
        vector = np.mean(np.exp(1j * np.deg2rad(centered[ok])), axis=1)
        a = np.rad2deg(np.angle(vector)) % 360
        a[a >= 360] = 0
        a[np.abs(vector) < 1e-9] = np.nan
        means[ok] = a
    signals['circular_mean'] = (x, means)
    return signals


def save_profile(p, stem):
    stderr = np.divide(p.std, np.sqrt(p.count), out=np.full(len(p.count), np.nan), where=p.count > 1)
    pd.DataFrame({'relative_x': p.relative_x, 'mean_deg': p.mean, 'std_deg': p.std,
                  'stderr_deg': stderr, 'count': p.count}).to_csv(str(stem) + '.csv', index=False)
    native, summary = [], []
    for k, (xr, xa, r) in enumerate(p.native_rows):
        native.append(pd.DataFrame({'period': k, 'relative_x': xr, 'absolute_x': xa, 'residual_deg': r}))
        summary.append({'period': k, 'start_x': p.crossings[k], 'stop_x': p.crossings[k + 1],
                        'duration_x': p.lengths[k], 'native_samples': len(r),
                        'residual_mean_deg': float(r.mean()) if len(r) else np.nan,
                        'global_slope': p.slope, 'trend_x_center': p.x_center, 'trend_y_center': p.y_center})
    pd.concat(native, ignore_index=True).to_csv(str(stem) + '_native.csv', index=False)
    pd.DataFrame(summary).to_csv(str(stem) + '_periods.csv', index=False)
    pd.DataFrame({'relative_x': p.relative_x, **{f'period_{k}': r for k, r in enumerate(p.rows)}}).to_csv(
        str(stem) + '_overlay.csv', index=False)


def plot_result(p, name, path):
    import matplotlib.pyplot as plt
    fig, axes = plt.subplots(3, 1, figsize=(12, 10), sharex=True, constrained_layout=True)
    axes[0].plot(p.relative_x, p.mean, 'k', label='Среднее')
    axes[0].fill_between(p.relative_x, p.mean - p.std, p.mean + p.std, alpha=.25, label='±СКО')
    axes[0].set_title(f'{name}: средний период={p.period_samples:.6g}; оборотов={p.n_periods}; один глобальный тренд')
    axes[0].legend()
    axes[1].plot(p.relative_x, p.std)
    axes[1].set_ylabel('СКО, град')
    for xr, _, residual in p.native_rows:
        axes[2].plot(xr, residual, lw=.5, alpha=.5)
    axes[2].plot(p.relative_x, p.mean, 'k', lw=2)
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
    p.add_argument('--grid-step', type=float)
    p.add_argument('--offsets', help='Смещения для кругового среднего в порядке возрастания ID')
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
    offsets = None
    if args.offsets:
        offsets = np.asarray([float(v) for v in args.offsets.split(',')])
        if len(offsets) != len(columns) or not np.isfinite(offsets).all():
            raise ValueError('Нужно одно конечное смещение на угловой столбец')
    print(f'CSV: {path}')
    signals = build_signals(df, columns, offsets, args.x)
    successes = 0
    for name, (x, angle) in signals.items():
        try:
            p = analyze(x, angle, args.grid_step)
        except ValueError as exc:
            print(f'{name}: {exc}', file=sys.stderr)
            continue
        unit = 'с' if args.x.endswith(('_us', '_ms')) else args.x
        p.x_label = unit + ' от фактического начала оборота'
        stem = path.with_name(f'{path.stem}_{name}_profile')
        save_profile(p, stem)
        print(f'{name}: P={p.period_samples:.9g} {unit}; оборотов={p.n_periods}; '
              f'Pmin={p.lengths.min():.9g}; Pmax={p.lengths.max():.9g}')
        if not args.no_plots:
            fig = plot_result(p, name, str(stem) + '.png')
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
