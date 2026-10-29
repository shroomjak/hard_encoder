#!/usr/bin/env python3
"""Глобальные остатки и межканальные разности головок: Huber RLM (statsmodels)."""
import argparse
from itertools import combinations
from pathlib import Path
import re

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import statsmodels.api as sm


def resolve_csv(value):
    """Имя эксперимента в diagnostics, путь к папке или путь к CSV."""
    base = Path(__file__).resolve().parent
    text = str(value).strip().strip('"')
    if not text:
        raise ValueError('Не указана папка эксперимента или CSV')
    direct_folder = base / text
    direct_csv = direct_folder / f'{direct_folder.name}.csv'
    if direct_csv.is_file():
        return direct_csv.resolve()

    p = Path(text.replace('\\', '/')).expanduser()
    if p.is_absolute():
        candidates = [p]
    elif len(p.parts) == 1:
        stem = p.stem if p.suffix.lower() == '.csv' else p.name
        candidates = [base / p, base / stem / f'{stem}.csv', base / f'{stem}.csv', p]
    else:
        candidates = [p, base / p, base.parent / p]

    tried = [str(direct_csv)]
    for candidate in candidates:
        if candidate.is_dir():
            candidate = candidate / f'{candidate.name}.csv'
        elif not candidate.suffix:
            expected = candidate / f'{candidate.name}.csv'
            if expected.is_file():
                candidate = expected
        if candidate.is_file():
            return candidate.resolve()
        tried.append(str(candidate))
    raise FileNotFoundError('CSV не найден. Проверены пути:\n  ' + '\n  '.join(dict.fromkeys(tried)))


def unwrap_seq_u16(seq):
    """Разворачивает переполнение uint16 счётчика seq (65535 -> 0), не меняя короткие записи."""
    seq = np.asarray(seq, dtype=float)
    out = seq.copy()
    finite = np.flatnonzero(np.isfinite(seq))
    if len(finite) > 1:
        vals = seq[finite]
        d = np.diff(vals)
        wraps = np.where(d < -32768.0, 65536.0, 0.0)
        if np.any(wraps):
            out[finite[1:]] = vals[1:] + np.cumsum(wraps)
    return out


def residual(x, y):
    """Остаток относительно линейного тренда Huber RLM (с защитой от нулевого масштаба)."""
    x = np.asarray(x, dtype=float)
    y = np.asarray(y, dtype=float)
    if len(x) == 0:
        return np.array([], dtype=float)
    scale = x.std()
    if not np.isfinite(scale) or scale == 0.0 or len(x) < 3:
        return y - np.median(y)
    z = (x - x.mean()) / scale
    coef = np.polyfit(z, y, 1)
    ols_resid = y - np.polyval(coef, z)
    if np.median(np.abs(ols_resid - np.median(ols_resid))) < 1e-12:
        return ols_resid
    design = sm.add_constant(z)
    try:
        res = sm.RLM(y, design, M=sm.robust.norms.HuberT()).fit().resid
        return res if np.isfinite(res).all() else ols_resid
    except (ValueError, ZeroDivisionError, np.linalg.LinAlgError):
        return ols_resid


def huber_center(y):
    """Центрирование константой по Huber RLM (без удаления наклона)."""
    y = np.asarray(y, dtype=float)
    if len(y) == 0:
        return np.array([], dtype=float), np.nan
    med = float(np.median(y))
    if len(y) < 3 or np.allclose(y, med):
        return y - med, med
    try:
        fit = sm.RLM(y, np.ones((len(y), 1)), M=sm.robust.norms.HuberT()).fit()
        center = float(fit.params[0])
        if np.isfinite(center):
            return y - center, center
    except (ValueError, ZeroDivisionError, np.linalg.LinAlgError):
        pass
    return y - med, med


def huber_linear_trend(x, y):
    """Линейный тренд Huber RLM для визуализации медленного дрейфа разности."""
    x = np.asarray(x, dtype=float)
    y = np.asarray(y, dtype=float)
    r = residual(x, y)
    return y - r


def valid_angle_series(df, column, base_mask):
    """Возвращает массив угла и маску валидных отсчётов [0, 360) с учётом valid_<ID>."""
    angle = pd.to_numeric(df[column], errors='coerce').to_numpy(dtype=float)
    mask = base_mask & np.isfinite(angle) & (angle >= 0.0) & (angle < 360.0)
    hid = column.split('_')[-1]
    valid_col = f'valid_{hid}'
    if valid_col in df:
        mask &= pd.to_numeric(df[valid_col], errors='coerce').to_numpy(dtype=float) == 1.0
    return angle, mask


def circular_pair_difference(ang_a, ang_b):
    """Вычисляет круговую разность (ang_a - ang_b) без скачков на 0/360° и вблизи ±180°.

    Возвращает:
      centered_deg : центрированная разность в градусах (нулевое робастное среднее),
      offset_signed: робастное среднее смещение в (-180, 180], град,
      offset_mod360: то же смещение в [0, 360), град.
    """
    ang_a = np.asarray(ang_a, dtype=float)
    ang_b = np.asarray(ang_b, dtype=float)
    rad = np.deg2rad(ang_a - ang_b)
    init_offset = float(np.rad2deg(np.arctan2(np.mean(np.sin(rad)), np.mean(np.cos(rad)))))
    wrapped = (ang_a - ang_b - init_offset + 180.0) % 360.0 - 180.0
    unwrapped = np.rad2deg(np.unwrap(np.deg2rad(wrapped)))
    centered, shift = huber_center(unwrapped)
    offset_signed = (init_offset + shift + 180.0) % 360.0 - 180.0
    if offset_signed <= -180.0:
        offset_signed += 360.0
    offset_mod360 = offset_signed % 360.0
    return centered, float(offset_signed), float(offset_mod360)


def fold_difference_by_angle(ang_ref, diff_deg, grid_step_deg=0.5, min_span_deg=340.0):
    """Раскладывает разность головок по отдельным оборотам угла ang_ref ∈ [0, 360).

    Возвращает словарь с равномерной угловой сеткой grid_deg, матрицей полных оборотов rows,
    средним профилем mean, межоборотным СКО std и нативными траекториями оборотов native_turns.
    """
    ang_ref = np.asarray(ang_ref, dtype=float)
    diff_deg = np.asarray(diff_deg, dtype=float)
    grid = np.arange(0.0, 360.0, float(grid_step_deg))

    unwrapped_ref = np.rad2deg(np.unwrap(np.deg2rad(ang_ref)))
    turn_idx = np.floor(unwrapped_ref / 360.0).astype(int)

    full_rows = []
    native_turns = []
    for k in np.unique(turn_idx):
        use = turn_idx == k
        a_k = ang_ref[use]
        d_k = diff_deg[use]
        if len(a_k) < 8:
            continue
        order = np.argsort(a_k)
        a_sorted = a_k[order]
        d_sorted = d_k[order]
        native_turns.append((a_sorted, d_sorted))

        span = float(a_sorted[-1] - a_sorted[0])
        if span < min_span_deg or len(a_sorted) < 24:
            continue
        uniq_a, inv = np.unique(a_sorted, return_inverse=True)
        if len(uniq_a) < 16:
            continue
        uniq_d = np.bincount(inv, weights=d_sorted) / np.bincount(inv)
        a_pad = np.concatenate(([uniq_a[-1] - 360.0], uniq_a, [uniq_a[0] + 360.0]))
        d_pad = np.concatenate(([uniq_d[-1]], uniq_d, [uniq_d[0]]))
        full_rows.append(np.interp(grid, a_pad, d_pad))

    if len(full_rows) >= 2:
        rows = np.vstack(full_rows)
        mean = np.mean(rows, axis=0)
        std = np.std(rows, axis=0, ddof=1)
        count = np.full(len(grid), len(full_rows), dtype=int)
    else:
        rows = np.empty((0, len(grid)), dtype=float)
        bins = np.floor(ang_ref / grid_step_deg).astype(int) % len(grid)
        count = np.bincount(bins, minlength=len(grid))
        sum_y = np.bincount(bins, weights=diff_deg, minlength=len(grid))
        mean = np.divide(sum_y, count, out=np.full(len(grid), np.nan), where=count > 0)
        diff_sq = (diff_deg - mean[bins]) ** 2
        sum_sq = np.bincount(bins, weights=np.where(np.isfinite(diff_sq), diff_sq, 0.0), minlength=len(grid))
        std = np.sqrt(np.divide(sum_sq, count - 1, out=np.full(len(grid), np.nan), where=count > 1))

    return {
        'grid_deg': grid,
        'rows': rows,
        'n_full_turns': int(rows.shape[0]),
        'mean': mean,
        'std': std,
        'count': count,
        'native_turns': native_turns,
    }


def save_standard_plot(fig, ax, folder, stem, suffix, title, xlabel, ylabel, subtitle='Глобальный тренд Huber'):
    ax.set(title=f'{title} — {stem}\n{subtitle}', xlabel=xlabel, ylabel=ylabel)
    ax.axhline(0, color='black', linewidth=.5, alpha=.5)
    ax.grid(alpha=.3)
    ax.legend()
    output = folder / f'{stem}_{suffix}.png'
    fig.savefig(output, dpi=150)
    print(output)


def plot_pair_angle_profile(folded, col_a, col_b, offset_signed, offset_mod360, stem, output_path):
    """Строит 3-панельный пооборотный профиль разности (col_a - col_b) по углу col_a ∈ [0, 360)."""
    grid = folded['grid_deg']
    mean = folded['mean']
    std = folded['std']
    n_turns = folded['n_full_turns']
    prof_rms = float(np.sqrt(np.nanmean(mean ** 2))) if np.isfinite(mean).any() else np.nan
    inter_turn_std = float(np.sqrt(np.nanmean(std ** 2))) if np.isfinite(std).any() else np.nan

    fig, axes = plt.subplots(3, 1, figsize=(12, 10), sharex=True, layout='constrained')
    axes[0].plot(grid, mean, color='black', linewidth=1.4, label='Средний профиль разности')
    if np.isfinite(std).any():
        axes[0].fill_between(grid, mean - std, mean + std, alpha=.25, label='±СКО между оборотами')
    axes[0].axhline(0, color='black', linewidth=.5, alpha=.5)
    axes[0].set_title(
        f'{col_a} − {col_b} по углу {col_a} — {stem}\n'
        f'Смещение={offset_signed:+.4f}° ({offset_mod360:.4f}°); полных оборотов={n_turns}; '
        f'СКО профиля={prof_rms:.5f}° ({prof_rms * 3600:.1f}"); '
        f'межоборотное СКО={inter_turn_std:.5f}° ({inter_turn_std * 3600:.1f}")'
    )
    axes[0].set_ylabel('Разность, град')
    axes[0].legend()

    axes[1].plot(grid, std, linewidth=1.0)
    axes[1].set_ylabel('СКО оборотов, град')

    for a_turn, d_turn in folded['native_turns']:
        axes[2].plot(a_turn, d_turn, linewidth=.5, alpha=.5)
    axes[2].plot(grid, mean, color='black', linewidth=1.8, label='Среднее')
    axes[2].axhline(0, color='black', linewidth=.5, alpha=.5)
    axes[2].set_ylabel('Разность, град')
    axes[2].set_xlabel(f'Угол {col_a}, град')
    axes[2].set_xlim(0.0, 360.0)
    axes[2].set_xticks(np.arange(0.0, 361.0, 30.0))

    for ax in axes:
        ax.grid(alpha=.3)

    fig.savefig(output_path, dpi=150)
    print(output_path)
    return fig, prof_rms, inter_turn_std


def parse_head_columns(df, ids_arg):
    if ids_arg:
        ids = [int(i) for part in ids_arg for i in re.split(r'[,\s]+', part) if i]
        return [f'ang_{i}' for i in ids]
    return sorted(
        (c for c in df if re.fullmatch(r'ang_\d+', str(c))),
        key=lambda c: int(c.split('_')[1]),
    )


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('folder', help='Папка эксперимента внутри diagnostics или путь к CSV')
    parser.add_argument('--ids', '--heads', nargs='+', help='Например: 1,2 или 1 2; по умолчанию все')
    parser.add_argument('--no-show', action='store_true', help='Не открывать интерактивные окна графиков')
    args = parser.parse_args()

    path = resolve_csv(args.folder)
    folder = path.parent
    stem = path.stem

    df = pd.read_csv(path)
    df.columns = [str(c).strip() for c in df.columns]
    seq = unwrap_seq_u16(pd.to_numeric(df['seq'], errors='coerce').to_numpy(dtype=float))
    t_raw = pd.to_numeric(df['t_us'], errors='coerce').to_numpy(dtype=float)
    finite_t = np.flatnonzero(np.isfinite(t_raw))
    if len(finite_t) == 0:
        raise ValueError('В столбце t_us нет конечных значений')
    t = (t_raw - t_raw[finite_t[0]]) / 1e6
    columns = parse_head_columns(df, args.ids)
    if not columns:
        raise ValueError('В CSV не найдены столбцы ang_<ID>')

    fig_seq, ax_seq = plt.subplots(figsize=(12, 5), layout='constrained')
    fig_dt, ax_dt = plt.subplots(figsize=(12, 5), layout='constrained')
    fig_time, ax_time = plt.subplots(figsize=(12, 5), layout='constrained')
    fig_ang, ax_ang = plt.subplots(figsize=(12, 5), layout='constrained')

    valid_time = np.isfinite(seq) & np.isfinite(t)
    dt = np.full(len(df), np.nan)
    dt[valid_time] = 1000.0 * residual(seq[valid_time], t[valid_time])
    ax_dt.plot(seq, dt, linewidth=.8, label='t − Huber(t, seq)')

    head_angles = {}
    head_valid = {}
    head_r_time = {}

    for column in columns:
        angle, valid = valid_angle_series(df, column, valid_time)
        head_angles[column] = angle
        head_valid[column] = valid
        r_seq = np.full(len(df), np.nan)
        r_time = np.full(len(df), np.nan)
        if valid.any():
            unwrapped = np.rad2deg(np.unwrap(np.deg2rad(angle[valid])))
            r_seq[valid] = residual(seq[valid], unwrapped)
            r_time[valid] = residual(t[valid], unwrapped)
        head_r_time[column] = r_time
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
    for fig, ax, suffix, title, xlabel, ylabel in plots:
        save_standard_plot(fig, ax, folder, stem, suffix, title, xlabel, ylabel)

    pairs = list(combinations(columns, 2))
    if pairs:
        fig_dseq, ax_dseq = plt.subplots(figsize=(12, 5), layout='constrained')
        fig_dtime, ax_dtime = plt.subplots(figsize=(12, 5), layout='constrained')
        fig_dshaft, ax_dshaft = plt.subplots(figsize=(12, 5), layout='constrained')
        has_any_pair = False

        sync_entries = []
        for col_a, col_b in pairs:
            valid_pair = head_valid[col_a] & head_valid[col_b]
            if np.count_nonzero(valid_pair) < 4:
                continue
            has_any_pair = True
            id_a, id_b = col_a.split('_')[1], col_b.split('_')[1]
            pair_label = f'{col_a} − {col_b}'

            centered, off_signed, off_360 = circular_pair_difference(
                head_angles[col_a][valid_pair], head_angles[col_b][valid_pair]
            )
            diff_full = np.full(len(df), np.nan)
            diff_full[valid_pair] = centered

            trend_seq = np.full(len(df), np.nan)
            trend_vals = huber_linear_trend(seq[valid_pair], centered)
            trend_seq[valid_pair] = trend_vals
            drift_deg = float(trend_vals[-1] - trend_vals[0])
            std_deg = float(np.std(centered, ddof=1)) if len(centered) > 1 else 0.0

            stat_label = (
                f'{pair_label} (смещ. {off_signed:+.3f}° / {off_360:.3f}°, '
                f'СКО={std_deg:.5f}° [{std_deg * 3600:.1f}"], дрейф={drift_deg:+.4f}°)'
            )
            line_seq, = ax_dseq.plot(seq, diff_full, linewidth=.7, label=stat_label)
            ax_dseq.plot(seq, trend_seq, linestyle='--', linewidth=1.1, color=line_seq.get_color(), alpha=.85)

            line_time, = ax_dtime.plot(t, diff_full, linewidth=.7, label=stat_label)
            trend_time = np.full(len(df), np.nan)
            trend_time[valid_pair] = huber_linear_trend(t[valid_pair], centered)
            ax_dtime.plot(t, trend_time, linestyle='--', linewidth=1.1, color=line_time.get_color(), alpha=.85)

            common_shaft = 0.5 * (head_r_time[col_a] + head_r_time[col_b])
            ax_dshaft.scatter(common_shaft[valid_pair], centered, s=0.2, alpha=.4, label=pair_label)

            folded = fold_difference_by_angle(head_angles[col_a][valid_pair], centered)
            angle_suffix = 'head_diff_vs_angle' if len(pairs) == 1 else f'head_diff_{id_a}_{id_b}_vs_angle'
            _, prof_rms, inter_turn_std = plot_pair_angle_profile(
                folded, col_a, col_b, off_signed, off_360, stem, folder / f'{stem}_{angle_suffix}.png'
            )

            print(
                f'[{pair_label}] смещение={off_signed:+.6f}° ({off_360:.6f}°); '
                f'СКО={std_deg:.6f}° ({std_deg * 3600:.2f}"); '
                f'СКО профиля={prof_rms:.6f}° ({prof_rms * 3600:.2f}"); '
                f'межоборотное СКО={inter_turn_std:.6f}° ({inter_turn_std * 3600:.2f}"); '
                f'линейный дрейф={drift_deg:+.6f}°'
            )

            age_col_a, age_col_b = f'age_us_{id_a}', f'age_us_{id_b}'
            raw_col_a, raw_col_b = f'raw_ang_{id_a}', f'raw_ang_{id_b}'
            has_age = age_col_a in df and age_col_b in df
            has_raw = raw_col_a in df and raw_col_b in df
            if has_age or has_raw:
                sync_entries.append((id_a, id_b, pair_label, valid_pair, diff_full, std_deg))

        if has_any_pair:
            save_standard_plot(
                fig_dseq, ax_dseq, folder, stem, 'head_diff_vs_seq',
                'Разность углов головок по seq', 'seq', 'Разность (центрир.), град',
                subtitle='Круговая разность без удаления скорости вала (пунктир — линейный тренд Huber)',
            )
            save_standard_plot(
                fig_dtime, ax_dtime, folder, stem, 'head_diff_vs_time',
                'Разность углов головок по времени', 'Время, с', 'Разность (центрир.), град',
                subtitle='Круговая разность без удаления скорости вала (пунктир — линейный тренд Huber)',
            )
            save_standard_plot(
                fig_dshaft, ax_dshaft, folder, stem, 'head_diff_vs_shaft_residual',
                'Разность углов головок по общему остатку вала',
                'Общий угловой остаток 0.5·(r_i + r_j), град', 'Разность (центрир.), град',
                subtitle='Проверка независимости разности головок от неравномерности привода',
            )
        else:
            plt.close(fig_dseq)
            plt.close(fig_dtime)
            plt.close(fig_dshaft)

        if sync_entries:
            has_age_data = False
            has_raw_distinct = False
            has_corr_data = False
            fig_sync, (ax_age, ax_raw, ax_corr) = plt.subplots(3, 1, figsize=(12, 9), sharex=True, layout='constrained')
            for id_a, id_b, pair_label, valid_pair, comp_diff_full, comp_std in sync_entries:
                age_col_a, age_col_b = f'age_us_{id_a}', f'age_us_{id_b}'
                if age_col_a in df and age_col_b in df:
                    age_a = pd.to_numeric(df[age_col_a], errors='coerce').to_numpy(dtype=float)
                    age_b = pd.to_numeric(df[age_col_b], errors='coerce').to_numpy(dtype=float)
                    ok_age = valid_pair & np.isfinite(age_a) & np.isfinite(age_b)
                    if ok_age.any() and (np.any(age_a[ok_age] != 0) or np.any(age_b[ok_age] != 0)):
                        d_age = np.full(len(df), np.nan)
                        d_age[ok_age] = age_a[ok_age] - age_b[ok_age]
                        age_mean = float(np.mean(d_age[ok_age]))
                        age_std = float(np.std(d_age[ok_age], ddof=1)) if np.count_nonzero(ok_age) > 1 else 0.0
                        ax_age.plot(
                            seq, d_age, linewidth=.7,
                            label=f'age_us_{id_a} − age_us_{id_b} (средн. {age_mean:+.1f} мкс, СКО={age_std:.1f} мкс)',
                        )
                        has_age_data = True

                raw_col_a, raw_col_b = f'raw_ang_{id_a}', f'raw_ang_{id_b}'
                if raw_col_a in df and raw_col_b in df:
                    raw_a, ok_ra = valid_angle_series(df, raw_col_a, valid_pair)
                    raw_b, ok_rb = valid_angle_series(df, raw_col_b, valid_pair)
                    ok_raw = ok_ra & ok_rb
                    if ok_raw.any():
                        differ = (
                            not np.allclose(raw_a[ok_raw], head_angles[f'ang_{id_a}'][ok_raw], atol=1e-7)
                            or not np.allclose(raw_b[ok_raw], head_angles[f'ang_{id_b}'][ok_raw], atol=1e-7)
                        )
                        if differ or has_age_data:
                            raw_centered, _, _ = circular_pair_difference(raw_a[ok_raw], raw_b[ok_raw])
                            raw_full = np.full(len(df), np.nan)
                            raw_full[ok_raw] = raw_centered
                            raw_std = float(np.std(raw_centered, ddof=1)) if len(raw_centered) > 1 else 0.0
                            ax_raw.plot(
                                seq, raw_full, linewidth=.6, alpha=.65,
                                label=f'raw_{id_a} − raw_{id_b} до комп. (СКО={raw_std:.5f}° [{raw_std * 3600:.1f}"])',
                            )
                            ax_raw.plot(
                                seq, comp_diff_full, linewidth=.7,
                                label=f'{pair_label} после комп. (СКО={comp_std:.5f}° [{comp_std * 3600:.1f}"])',
                            )
                            has_raw_distinct = True

                            corr_arcsec = np.full(len(df), np.nan)
                            corr_vals = ((comp_diff_full[ok_raw] - raw_centered + 180.0) % 360.0 - 180.0) * 3600.0
                            corr_arcsec[ok_raw] = corr_vals
                            corr_std = float(np.std(corr_vals, ddof=1)) if len(corr_vals) > 1 else 0.0
                            ax_corr.plot(
                                seq, corr_arcsec, linewidth=.7,
                                label=f'Поправка ({pair_label}) − (raw_{id_a} − raw_{id_b}) (СКО={corr_std:.2f}")',
                            )
                            has_corr_data = True

            if has_age_data or has_raw_distinct:
                ax_age.set_title(f'Диагностика синхронизации кадров и компенсации age_us — {stem}')
                ax_age.set_ylabel('Δage_us, мкс')
                ax_age.axhline(0, color='black', linewidth=.5, alpha=.5)
                ax_age.grid(alpha=.3)
                if has_age_data:
                    ax_age.legend()
                ax_raw.set_ylabel('Разность углов, град')
                ax_raw.axhline(0, color='black', linewidth=.5, alpha=.5)
                ax_raw.grid(alpha=.3)
                if has_raw_distinct:
                    ax_raw.legend()
                ax_corr.set_ylabel('Поправка комп., угл. с')
                ax_corr.set_xlabel('seq')
                ax_corr.axhline(0, color='black', linewidth=.5, alpha=.5)
                ax_corr.grid(alpha=.3)
                if has_corr_data:
                    ax_corr.legend()
                out_sync = folder / f'{stem}_head_sync_diagnostics.png'
                fig_sync.savefig(out_sync, dpi=150)
                print(out_sync)
            else:
                plt.close(fig_sync)

    if args.no_show:
        plt.close('all')
    else:
        plt.show()


if __name__ == '__main__':
    main()
