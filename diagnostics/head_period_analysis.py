#!/usr/bin/env python3
"""head_period_analysis.py — диагностические графики по log_*.csv.

Входной CSV (результат serial_head_logger.py):
    seq,ang_1,...,ang_k,status

Для каждой головки (столбец ang_i) и для кругового среднего арифметического
показаний головок строится отдельный график в стиле period_analysis.py:
отсечение постоянных участков, оценка периода, выделение полных периодов,
глобальный линейный детренд, наложение периодов, среднее и СКО.

Результаты (k+1 рисунок) сохраняются в папку с тем же именем, что и лог
(без расширения), например log_foo/ .
"""

from __future__ import annotations

import argparse
import os
import sys
import tempfile
from pathlib import Path

import numpy as np
import pandas as pd

HERE = Path(__file__).resolve().parent
if str(HERE) not in sys.path:
    sys.path.insert(0, str(HERE))
import period_analysis as pa


def _read_log(path: Path) -> pd.DataFrame:
    df = pd.read_csv(path)
    # Нормализуем имена столбцов.
    df.columns = [str(c).strip() for c in df.columns]
    return df


def _angle_columns(df: pd.DataFrame) -> list[str]:
    cols = [c for c in df.columns if c.lower().startswith("ang")]
    # На случай ang_1 ... ang_k; сохраняем порядок.
    if not cols:
        # fallback: все кроме seq/status
        cols = [c for c in df.columns if c.lower() not in ("seq", "status", "idx")]
    return cols


def _run_period_analysis(x: np.ndarray, angle: np.ndarray, std: np.ndarray, args) -> pa.PeriodAnalysisResult:
    """Робастный расчёт на сырых данных: без сглаживания и эвристик."""
    x = np.asarray(x, dtype=float).ravel()
    angle = np.asarray(angle, dtype=float).ravel()
    std = np.asarray(std, dtype=float).ravel()

    order = np.argsort(x)
    x = x[order]; angle = angle[order]; std = std[order]

    wrapped = np.mod(angle, 360.0)
    unwrapped = pa.unwrap_deg(wrapped)

    # --- 1. Отсечение постоянных участков (без сглаживания) ---
    if len(x) >= 3:
        dx = np.diff(x)
        dx_safe = np.where(dx == 0, np.nan, dx)
        speed = np.abs(pa.angle_diff(wrapped[1:], wrapped[:-1]) / dx_safe)
        finite_speed = speed[np.isfinite(speed)]
        if finite_speed.size and np.nanmax(finite_speed) > 0:
            thr = max(1e-9, 0.05 * float(np.nanmax(finite_speed)))
            active = np.isfinite(speed) & (speed >= thr)
            idx = np.where(active)[0]
            if idx.size >= 2:
                s = int(idx[0])
                e = int(idx[-1]) + 1
                e = min(e, len(x) - 1)
                x = x[s:e + 1]
                angle = angle[s:e + 1]
                std = std[s:e + 1]
                wrapped = wrapped[s:e + 1]
                unwrapped = unwrapped[s:e + 1]

    # --- 2. Линейный тренд по сырым данным (МНК, без робастного клиппинга) ---
    slope, intercept = np.polyfit(x, unwrapped, 1)
    if abs(slope) < 1e-12:
        raise ValueError("Наклон сигнала ~0, невозможно определить период")
    period = 360.0 / abs(slope)

    if args.min_period is not None and period < args.min_period:
        period = float(args.min_period)
    if args.max_period is not None and period > args.max_period:
        period = float(args.max_period)

    dx_med = float(np.median(np.diff(x))) if len(x) > 1 else 1.0
    if args.n_phase is None:
        n_phase = max(16, int(round(period / dx_med))) if dx_med > 0 else 16
    else:
        n_phase = int(args.n_phase)

    # --- 3. Границы полных периодов ---
    boundary_x = pa.find_period_boundaries(x, float(slope), float(intercept))
    if boundary_x.size < 2:
        raise ValueError(f"Недостаточно полных периодов: границ {boundary_x.size}")

    n_periods = int(boundary_x.size - 1)
    period_starts = boundary_x[:-1]
    period_ends = boundary_x[1:]
    actual_periods = np.diff(boundary_x)
    period_refined = float(np.median(actual_periods))

    # --- 4. Фолдинг + глобальный линейный детренд ---
    phase_frac = (np.arange(n_phase) + 0.5) / n_phase
    angle_mat = np.empty((n_periods, n_phase))
    std_mat = np.empty((n_periods, n_phase))
    query_x_mat = np.empty((n_periods, n_phase))

    for k in range(n_periods):
        xs = float(period_starts[k])
        xe = float(period_ends[k])
        local_period = xe - xs
        query_x = xs + phase_frac * local_period
        query_x_mat[k] = query_x
        angle_mat[k] = np.interp(query_x, x, unwrapped)
        std_mat[k] = np.interp(query_x, x, std)

    trend = float(slope) * query_x_mat + float(intercept)
    error_mat = angle_mat - trend

    # Без отсечения периодов-выбросов: все полные периоды участвуют.
    period_mask = np.ones(n_periods, dtype=bool)
    valid_errors = error_mat[period_mask]
    valid_stds = std_mat[period_mask]
    n_valid = valid_errors.shape[0]

    mean_err = np.mean(valid_errors, axis=0)
    if n_valid > 1:
        stat_std = np.std(valid_errors, axis=0, ddof=1)
    else:
        stat_std = np.zeros(n_phase)
    single_std = np.sqrt(np.mean(valid_stds ** 2, axis=0))
    total_std = np.sqrt(stat_std ** 2 + single_std ** 2)

    phase_samples = phase_frac * period_refined
    phase_deg = phase_frac * 360.0

    slope_sign = float(np.sign(slope))
    if slope_sign < 0:
        phase_deg = 360.0 - phase_frac * 360.0
        sort_idx = np.argsort(phase_deg)
        phase_deg = phase_deg[sort_idx]
        phase_samples = phase_samples[sort_idx]
        mean_err = mean_err[sort_idx]
        stat_std = stat_std[sort_idx]
        single_std = single_std[sort_idx]
        total_std = total_std[sort_idx]
        error_mat = error_mat[:, sort_idx]

    return pa.PeriodAnalysisResult(
        period_samples=period_refined,
        n_periods=n_valid,
        phase_samples=phase_samples,
        phase_deg=phase_deg,
        mean_error=mean_err,
        statistical_std=stat_std,
        single_frame_std=single_std,
        total_std=total_std,
        error_matrix=error_mat[period_mask],
        period_mask=period_mask,
        fitted_slope=float(slope),
        rotation_direction="increasing" if slope_sign >= 0 else "decreasing",
        active_range=(float(x[0]), float(x[-1])),
        period_boundaries=boundary_x,
    )


def _circular_mean_deg(matrix: np.ndarray) -> np.ndarray:
    """Круговое среднее арифметическое углов (град) по строкам matrix (N x k)."""
    rad = np.deg2rad(matrix)
    ms = np.mean(np.sin(rad), axis=1)
    mc = np.mean(np.cos(rad), axis=1)
    return np.rad2deg(np.arctan2(ms, mc)) % 360.0


def run(args: argparse.Namespace) -> None:
    csv_path = Path(args.csv)
    if not csv_path.exists():
        print(f"[ERROR] Файл не найден: {csv_path}", file=sys.stderr)
        sys.exit(1)

    df = _read_log(csv_path)
    if "seq" not in [c.lower() for c in df.columns]:
        # попробуем первый столбец как seq
        seq_col = df.columns[0]
    else:
        seq_col = [c for c in df.columns if c.lower() == "seq"][0]

    ang_cols = _angle_columns(df)
    if not ang_cols:
        print("[ERROR] Не найдены столбцы ang_*", file=sys.stderr)
        sys.exit(1)

    # Фильтруем по status, если есть.
    if any(c.lower() == "status" for c in df.columns):
        status_col = [c for c in df.columns if c.lower() == "status"][0]
        mask = df[status_col].fillna(0).astype(int) == 1
        df = df.loc[mask].copy()

    x_all = df[seq_col].to_numpy(dtype=float)
    k = len(ang_cols)

    # Папка для результатов: имя лога без расширения.
    out_dir = csv_path.parent / csv_path.stem
    out_dir.mkdir(parents=True, exist_ok=True)
    print(f"[INFO] Результаты будут сохранены в: {out_dir}", flush=True)

    import matplotlib.pyplot as plt

    figures = []

    # k графиков по головкам.
    for i, col in enumerate(ang_cols, start=1):
        print(f"\n===== Головка {i} ({col}) =====", flush=True)
        angle = df[col].to_numpy(dtype=float)
        std = np.zeros_like(angle)
        # Пропускаем NaN.
        valid = ~np.isnan(angle)
        x = x_all[valid]
        a = angle[valid]
        s = std[valid]
        if len(x) < 5:
            print(f"[WARN] Недостаточно данных для {col}", flush=True)
            continue
        res = _run_period_analysis(x, a, s, args)
        out_path = out_dir / f"head_{i}.png"
        pa.plot_result(res, out_path=str(out_path))
        # Сохраняем CSV профиля.
        pa.save_csv(res, str(out_dir / f"head_{i}_profile.csv"))
        figures.append((f"Головка {i}", res))
        print(f"Сохранено: {out_path}", flush=True)

    # Круговое среднее.
    print("\n===== Круговое среднее головок =====", flush=True)
    mat = df[ang_cols].to_numpy(dtype=float)
    valid_rows = ~np.isnan(mat).any(axis=1)
    x_cm = x_all[valid_rows]
    mat_v = mat[valid_rows]
    cm_angle = _circular_mean_deg(mat_v)
    std_cm = np.zeros_like(cm_angle)
    if len(x_cm) >= 5:
        res_cm = _run_period_analysis(x_cm, cm_angle, std_cm, args)
        out_path = out_dir / "circular_mean.png"
        pa.plot_result(res_cm, out_path=str(out_path))
        pa.save_csv(res_cm, str(out_dir / "circular_mean_profile.csv"))
        print(f"Сохранено: {out_path}", flush=True)
    else:
        print("[WARN] Недостаточно данных для кругового среднего", flush=True)

    if not args.no_show:
        plt.show()


def build_argparser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(description="Диагностические графики по log_*.csv")
    p.add_argument("csv", help="Входной CSV log_*.csv")
    p.add_argument("--n-hint", type=float, default=10.0, dest="n_hint")
    p.add_argument("--min-period", type=float, default=None, dest="min_period")
    p.add_argument("--max-period", type=float, default=None, dest="max_period")
    p.add_argument("--n-phase", type=int, default=None, dest="n_phase")
    p.add_argument("--clip", type=float, default=3.0)
    p.add_argument("--no-filter", action="store_true", dest="no_filter")
    p.add_argument("--no-show", action="store_true", dest="no_show")
    return p


def main() -> None:
    args = build_argparser().parse_args()
    run(args)


if __name__ == "__main__":
    main()
