#!/usr/bin/env python3
"""Глобальные остатки: Huber RLM (statsmodels), без сглаживания и подгонки оборотов."""
import argparse
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import statsmodels.api as sm


def residual(x, y):
    x = np.asarray(x, dtype=float)
    y = np.asarray(y, dtype=float)
    design = sm.add_constant((x - x.mean()) / x.std())
    return sm.RLM(y, design, M=sm.robust.norms.HuberT()).fit().resid


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('folder', help='Папка эксперимента внутри diagnostics')
    parser.add_argument('--ids', '--heads', nargs='+', help='Например: 1,2 или 1 2; по умолчанию все')
    args = parser.parse_args()

    folder = Path(__file__).resolve().parent / args.folder
    path = folder / f'{folder.name}.csv'
    df = pd.read_csv(path)
    seq = df['seq'].to_numpy(dtype=float)
    t = (df['t_us'].to_numpy(dtype=float) - df['t_us'].iloc[0]) / 1e6
    columns = [f'ang_{int(i)}' for part in args.ids for i in part.split(',')] if args.ids else sorted(
        (c for c in df if c.startswith('ang_') and c[4:].isdigit()), key=lambda c: int(c[4:]))

    fig_seq, ax_seq = plt.subplots(figsize=(12, 5), layout='constrained')
    fig_dt, ax_dt = plt.subplots(figsize=(12, 5), layout='constrained')
    fig_time, ax_time = plt.subplots(figsize=(12, 5), layout='constrained')
    fig_ang, ax_ang = plt.subplots(figsize=(12, 5), layout='constrained')
    valid_time = np.isfinite(seq) & np.isfinite(t)
    dt = np.full(len(df), np.nan)
    dt[valid_time] = 1000 * residual(seq[valid_time], t[valid_time])
    ax_dt.plot(seq, dt, linewidth=.8, label='t − Huber(t, seq)')

    for column in columns:
        angle = df[column].to_numpy(dtype=float)
        valid = valid_time & np.isfinite(angle)
        unwrapped = np.rad2deg(np.unwrap(np.deg2rad(angle[valid])))
        r_seq = np.full(len(df), np.nan)
        r_time = np.full(len(df), np.nan)
        r_seq[valid] = residual(seq[valid], unwrapped)
        r_time[valid] = residual(t[valid], unwrapped)
        ax_seq.plot(seq, r_seq, linewidth=.8, label=column)
        ax_time.plot(t, r_time, linewidth=.8, label=column)
        ax_ang.scatter(r_seq, dt, s=0.1, label=column)

    plots = [
        (fig_seq, ax_seq, 'angle_residual_vs_seq', 'Угловой остаток по seq', 'seq', 'Остаток, град'),
        (fig_dt, ax_dt, 'time_residual_vs_seq', 'Временной остаток по seq', 'seq', 'Остаток, мс'),
        (fig_time, ax_time, 'angle_residual_vs_time', 'Угловой остаток по времени', 'Время, с', 'Остаток, град'),
        (fig_ang, ax_ang, 'angle_residual_vs_time_residual', 'Угловой остаток по временному остатку', 'Остаток, град', 'Остаток, мс')
    ]
    for fig, ax, suffix, title, xlabel, ylabel in plots:
        ax.set(title=f'{title} — {folder.name}\nГлобальный тренд Huber', xlabel=xlabel, ylabel=ylabel)
        ax.axhline(0, color='black', linewidth=.5, alpha=.5)
        ax.grid(alpha=.3)
        ax.legend()
        output = folder / f'{folder.name}_{suffix}.png'
        fig.savefig(output, dpi=150)
        print(output)
    plt.show()


if __name__ == '__main__':
    main()
