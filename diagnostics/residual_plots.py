#!/usr/bin/env python3
"""Глобальные остатки и разности головок: Huber RLM (statsmodels), без подгонки оборотов.
На графике разности по углу оборота исключённые секторы диска выделяются вертикальными
пунктирными линиями без вырезания исходных точек.
"""
import argparse
from itertools import combinations
from pathlib import Path
import re
import sys

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import statsmodels.api as sm

sys.path.insert(0, str(Path(__file__).resolve().parent))
from head_period_analysis import (  # noqa: E402
    BIT_TAB_SIZE,
    EXCLUDED_SECTORS,
    SECTOR_WIDTH_DEG,
    merge_sector_intervals_on_x,
    parse_sector_list,
)


def resolve_csv(value):
    """Папка эксперимента внутри diagnostics, путь к папке или к CSV."""
    base = Path(__file__).resolve().parent
    p = Path(str(value).strip().strip('"')).expanduser()
    for candidate in (base / p / f'{p.name}.csv', base / p, p / f'{p.name}.csv', p):
        if candidate.is_file():
            return candidate.resolve()
    raise FileNotFoundError(f'CSV не найден: {value}')


def huber_resid(y, x=None):
    """Остаток Huber RLM: относительно константы (x=None) или линейного тренда по x."""
    y = np.asarray(y, dtype=float)
    if x is None:
        design = np.ones((len(y), 1))
    else:
        x = np.asarray(x, dtype=float)
        scale = x.std()
        design = sm.add_constant((x - x.mean()) / scale if scale > 0 else np.zeros_like(x))
    ols_resid = y - design @ np.linalg.lstsq(design, y, rcond=None)[0]
    if len(y) < 3 or sm.robust.mad(ols_resid) < 1e-12:
        return ols_resid
    import warnings
    from statsmodels.tools.sm_exceptions import ConvergenceWarning
    with warnings.catch_warnings():
        warnings.simplefilter('ignore', ConvergenceWarning)
        res = sm.RLM(y, design, M=sm.robust.norms.HuberT()).fit()
    return res.resid if np.all(np.isfinite(res.resid)) else ols_resid


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('folder', help='Папка эксперимента внутри diagnostics или путь к CSV')
    parser.add_argument('--ids', '--heads', nargs='+', help='Например: 1,2 или 1 2; по умолчанию все')
    parser.add_argument('--exclude-sectors', default=None,
                        help=f'Список секторов 0..143 для пунктирной разметки (по умолчанию {EXCLUDED_SECTORS}; none — отключить)')
    parser.add_argument('--no-show', action='store_true', help='Не открывать окна графиков')
    args = parser.parse_args()

    excluded_sectors = parse_sector_list(args.exclude_sectors)
    path = resolve_csv(args.folder)
    folder, stem = path.parent, path.stem
    df = pd.read_csv(path)
    df.columns = [str(c).strip() for c in df.columns]

    seq = pd.to_numeric(df['seq'], errors='coerce').to_numpy(dtype=float, copy=True)
    finite_seq = np.isfinite(seq)
    if finite_seq.sum() > 1 and np.any(np.diff(seq[finite_seq]) < -32768.0):
        seq[finite_seq] = np.unwrap(seq[finite_seq], period=65536.0)

    t_us = pd.to_numeric(df['t_us'], errors='coerce').to_numpy(dtype=float, copy=True)
    t = (t_us - t_us[np.isfinite(t_us)][0]) / 1e6
    columns = (
        [f'ang_{int(i)}' for part in args.ids for i in re.split(r'[,\s]+', part) if i]
        if args.ids else sorted(
            (c for c in df if re.fullmatch(r'ang_\d+', c)), key=lambda c: int(c.split('_')[1])
        )
    )

    fig_seq, ax_seq = plt.subplots(figsize=(12, 5), layout='constrained')
    fig_dt, ax_dt = plt.subplots(figsize=(12, 5), layout='constrained')
    fig_time, ax_time = plt.subplots(figsize=(12, 5), layout='constrained')
    fig_ang, ax_ang = plt.subplots(figsize=(12, 5), layout='constrained')

    valid_time = np.isfinite(seq) & np.isfinite(t)
    dt = np.full(len(df), np.nan)
    dt[valid_time] = 1000.0 * huber_resid(t[valid_time], seq[valid_time])
    ax_dt.plot(seq, dt, linewidth=.8, label='t − Huber(t, seq)')

    angles, head_valid = {}, {}
    for column in columns:
        angle = pd.to_numeric(df[column], errors='coerce').to_numpy(dtype=float)
        valid = valid_time & np.isfinite(angle) & (angle >= 0.0) & (angle < 360.0)
        valid_col = f"valid_{column.split('_')[1]}"
        if valid_col in df:
            valid &= pd.to_numeric(df[valid_col], errors='coerce').to_numpy() == 1
        angles[column], head_valid[column] = angle, valid
        if not valid.any():
            continue
        unwrapped = np.unwrap(angle[valid], period=360.0)
        r_seq = np.full(len(df), np.nan)
        r_time = np.full(len(df), np.nan)
        r_seq[valid] = huber_resid(unwrapped, seq[valid])
        r_time[valid] = huber_resid(unwrapped, t[valid])
        ax_seq.plot(seq, r_seq, linewidth=.8, label=column)
        ax_time.plot(t, r_time, linewidth=.8, label=column)
        ax_ang.scatter(r_seq, dt, s=0.1, label=column)

    plots = [
        (fig_seq, ax_seq, 'angle_residual_vs_seq', 'Угловой остаток по seq', 'seq', 'Остаток, град'),
        (fig_dt, ax_dt, 'time_residual_vs_seq', 'Временной остаток по seq', 'seq', 'Остаток, мс'),
        (fig_time, ax_time, 'angle_residual_vs_time', 'Угловой остаток по времени', 'Время, с', 'Остаток, град'),
        (fig_ang, ax_ang, 'angle_residual_vs_time_residual', 'Угловой остаток по временному остатку',
         'Остаток, град', 'Остаток, мс'),
    ]

    pairs = list(combinations(columns, 2))
    if pairs:
        fig_dtime, ax_dtime = plt.subplots(figsize=(12, 5), layout='constrained')
        fig_dang, ax_dang = plt.subplots(figsize=(12, 5.5), layout='constrained')
        plotted_pairs = 0
        pair_offsets: list[tuple[str, float]] = []
        for col_a, col_b in pairs:
            valid = head_valid[col_a] & head_valid[col_b]
            if valid.sum() < 4:
                continue
            plotted_pairs += 1
            raw_diff = np.unwrap(angles[col_a][valid] - angles[col_b][valid], period=360.0)
            d = huber_resid(raw_diff)
            offset = (float(np.median(raw_diff - d)) + 180.0) % 360.0 - 180.0
            std = float(d.std(ddof=1))
            label = f'{col_a} − {col_b} (смещ. {offset:+.3f}°, СКО={std:.5f}° [{std * 3600:.1f}"])'

            d_full = np.full(len(df), np.nan)
            d_full[valid] = d
            ax_dtime.plot(t, d_full, linewidth=.7, label=label)

            ang_ref = angles[col_a][valid]
            stat = pd.Series(d).groupby(np.floor(ang_ref * 2.0) / 2.0 + 0.25).agg(['mean', 'std'])
            ax_dang.scatter(ang_ref, d, s=0.2, alpha=.2)
            line, = ax_dang.plot(stat.index, stat['mean'], linewidth=1.2, label=label)
            ax_dang.fill_between(
                stat.index, stat['mean'] - stat['std'], stat['mean'] + stat['std'],
                color=line.get_color(), alpha=.2,
            )

            rel_ba = float(np.median(np.mod(angles[col_b][valid] - angles[col_a][valid], 360.0)))
            nom_ba = round(rel_ba / 90.0) * 90.0 % 360.0
            if not pair_offsets:
                pair_offsets.append((col_a, 0.0))
            if all(name != col_b for name, _ in pair_offsets):
                pair_offsets.append((col_b, nom_ba))

        if plotted_pairs:
            if excluded_sectors:
                palette = ['#d62728', '#ff7f0e', '#9467bd', '#2ca02c']
                used_labels: set[str] = set()
                for idx, (h_name, h_off) in enumerate(pair_offsets):
                    color = palette[idx % len(palette)]
                    for x0, x1, sec_label in merge_sector_intervals_on_x(excluded_sectors, h_off):
                        lbl = f'Искл. {sec_label} под {h_name} [{x0:.1f}°..{x1:.1f}°]'
                        ax_dang.axvline(x0, color=color, ls='--', lw=1.2, alpha=0.85,
                                        label=(lbl if lbl not in used_labels else None))
                        ax_dang.axvline(x1, color=color, ls='--', lw=1.2, alpha=0.85)
                        ax_dang.axvspan(x0, x1, color=color, alpha=0.07)
                        used_labels.add(lbl)

            ax_dang.set_xlim(0.0, 360.0)
            tick_angles = np.arange(0.0, 361.0, 30.0)
            ax_dang.set_xticks(tick_angles)
            top = ax_dang.twiny()
            top.set_xlim(0.0, 360.0)
            top.set_xticks(tick_angles)
            top.set_xticklabels([
                str(int((BIT_TAB_SIZE - round((a % 360.0) / SECTOR_WIDTH_DEG)) % BIT_TAB_SIZE))
                for a in tick_angles
            ], fontsize=8)
            top.set_xlabel('Граница сектора диска по первой головке (0..143, 1 сектор = 2.5°)', fontsize=9)

            plots.extend([
                (fig_dtime, ax_dtime, 'head_diff_vs_time', 'Разность углов головок по времени',
                 'Время, с', 'Разность, град'),
                (fig_dang, ax_dang, 'head_diff_vs_angle', 'Разность углов головок по углу оборота',
                 'Угол первой головки пары, град', 'Разность, град'),
            ])
        else:
            plt.close(fig_dtime)
            plt.close(fig_dang)

    for fig, ax, suffix, title, xlabel, ylabel in plots:
        ax.set(title=f'{title} — {stem}\nHuber RLM', xlabel=xlabel, ylabel=ylabel)
        ax.axhline(0, color='black', linewidth=.5, alpha=.5)
        ax.grid(alpha=.3)
        ax.legend(fontsize=8.5)
        output = folder / f'{stem}_{suffix}.png'
        fig.savefig(output, dpi=150)
        print(output)

    if args.no_show:
        plt.close('all')
    else:
        plt.show()


if __name__ == '__main__':
    main()
