"""
Анализ периодического пилообразного сигнала абсолютного энкодера.

Алгоритм:
  1. Чтение CSV (index, angle, std).
  2. Отсечение постоянных участков в начале/конце.
  3. Оценка периода (грубая + уточнение по минимуму дисперсии).
  4. Определение границ ПОЛНЫХ периодов и отбрасывание неполных.
  5. Fold полных периодов на единую фазовую ось.
  6. Глобальный линейный детренд → матрица ошибок.
  7. Статистика: mean и std по периодам (поточечно).

Ключевые особенности:
  - Неполные периоды в начале и конце данных полностью исключаются.
  - Границы периодов определяются по пересечению линейного тренда с N×360°.
  - Фазовая сетка сдвинута на полшага, чтобы не попадать на границы.
  - Глобальный (единый) детренд для всех периодов.
  - Отсечение выбросов — на уровне целых периодов (опционально).
"""

from __future__ import annotations

from dataclasses import dataclass
from typing import Tuple

import argparse
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt


# ============================================================
# Результат анализа
# ============================================================
@dataclass
class PeriodAnalysisResult:
    period_samples: float
    n_periods: int
    phase_samples: np.ndarray   # фазовая координата внутри периода (в отсчётах)
    phase_deg: np.ndarray       # фазовая координата 0..360°

    mean_error: np.ndarray      # средняя ошибка энкодера, град
    statistical_std: np.ndarray # СКО между периодами, град
    single_frame_std: np.ndarray  # folded СКО одиночного измерения, град
    total_std: np.ndarray       # sqrt(stat² + single²), град

    error_matrix: np.ndarray    # ошибки всех периодов после детренда (n_periods × n_phase)
    period_mask: np.ndarray     # True = период использован в статистике

    fitted_slope: float         # град/отсчёт (знак = направление)
    rotation_direction: str     # "increasing" / "decreasing"
    active_range: Tuple[float, float]  # [x_start, x_end] активного участка
    period_boundaries: np.ndarray  # x-координаты границ периодов


# ============================================================
# Угловые утилиты
# ============================================================
def wrap_deg(a: np.ndarray) -> np.ndarray:
    return np.mod(a, 360.0)


def angle_diff(a: np.ndarray, b: np.ndarray) -> np.ndarray:
    """Кратчайшая разность углов a - b, результат в (-180, 180]."""
    return ((a - b + 180.0) % 360.0) - 180.0


def unwrap_deg(a: np.ndarray) -> np.ndarray:
    return np.rad2deg(np.unwrap(np.deg2rad(a)))


# ============================================================
# Робастный линейный fit (итеративный, MAD-based)
# ============================================================
def robust_polyfit1(x: np.ndarray, y: np.ndarray,
                    clip: float = 3.0, n_iter: int = 5):
    """Возвращает (slope, intercept, inlier_mask)."""
    if len(x) < 2:
        raise ValueError("Недостаточно точек для fit")

    mask = np.ones(len(x), dtype=bool)
    slope, intercept = np.polyfit(x, y, 1)

    for _ in range(n_iter):
        r = y - (slope * x + intercept)
        med = np.median(r[mask])
        mad = np.median(np.abs(r[mask] - med))
        s = max(1.4826 * mad, 1e-12)
        mask_new = np.abs(r - med) <= clip * s
        if np.count_nonzero(mask_new) < 3:
            break
        if np.array_equal(mask, mask_new):
            break
        mask = mask_new
        slope, intercept = np.polyfit(x[mask], y[mask], 1)

    return slope, intercept, mask


# ============================================================
# Чтение CSV
# ============================================================
def read_csv(path: str, sep: str = ","):
    df = pd.read_csv(path, sep=sep, header=None, comment="#", engine="python")
    df = df.iloc[:, :3].apply(pd.to_numeric, errors="coerce").dropna()
    df.columns = ["idx", "angle", "std"]
    df = df.sort_values("idx").reset_index(drop=True)
    x = df["idx"].values.astype(float)
    a = wrap_deg(df["angle"].values.astype(float))
    s = df["std"].values.astype(float)
    return x, a, s


# ============================================================
# Выделение активного участка (отсечение постоянных краёв)
# ============================================================
def trim_constants(x: np.ndarray, angle: np.ndarray, std: np.ndarray,
                   smooth_pct: float = 1.0, threshold_pct: float = 15.0):
    """
    Отсекает начальный и конечный участки, где угол почти не меняется.
    Возвращает обрезанные x, angle, std.
    """
    if len(x) < 5:
        return x, angle, std

    dx = np.diff(x)
    dx = np.where(dx == 0, 1e-12, dx)  # защита от деления на 0
    speed = np.abs(angle_diff(angle[1:], angle[:-1]) / dx)

    # Сглаживание скорости скользящим средним
    w = max(3, int(len(speed) * smooth_pct / 100) | 1)
    speed_s = np.convolve(speed, np.ones(w) / w, mode="same")

    q10, q90 = np.percentile(speed_s, [10, 90])
    thr = q10 + threshold_pct / 100.0 * (q90 - q10)
    active = speed_s >= max(thr, 1e-9)

    # Морфологическое закрытие мелких дыр
    for gap in range(1, w + 1):
        kernel = np.ones(2 * gap + 1) / (2 * gap + 1)
        active = np.convolve(active.astype(float), kernel, mode="same") > 0.5

    idx = np.where(active)[0]
    if len(idx) < 5:
        return x, angle, std

    # speed имеет длину N-1, speed[i] соответствует интервалу [x[i], x[i+1]].
    s = idx[0]
    e = idx[-1] + 1
    e = min(e, len(x) - 1)

    return x[s:e + 1], angle[s:e + 1], std[s:e + 1]


# ============================================================
# Оценка периода
# ============================================================
def estimate_period(x: np.ndarray, angle: np.ndarray,
                    n_hint: float = 10.0,
                    refine_frac: float = 0.05,
                    refine_n: int = 200) -> Tuple[float, float]:
    """
    Грубая оценка периода по наклону unwrapped сигнала + уточнение
    по минимуму дисперсии при фолдинге.

    Возвращает (period, slope).
    """
    u = unwrap_deg(angle)
    slope, intercept, _ = robust_polyfit1(x, u)

    if abs(slope) < 1e-12:
        raise ValueError("Наклон сигнала ≈ 0, невозможно определить период")

    p0 = 360.0 / abs(slope)
    span = x[-1] - x[0]

    lo = max(p0 * (1 - refine_frac), span / (n_hint * 2))
    hi = min(p0 * (1 + refine_frac), span / max(n_hint / 2, 1))
    if lo >= hi:
        return p0, slope

    dx = np.median(np.diff(x))
    nph = int(np.clip(p0 / dx, 64, 512))

    best_p, best_score = p0, np.inf

    for p in np.linspace(lo, hi, refine_n):
        n_full = int((span - dx) / p)
        if n_full < 2:
            continue

        phase_frac = (np.arange(nph) + 0.5) / nph
        offset = (span - n_full * p) / 2
        starts = x[0] + offset + np.arange(n_full) * p
        query_x = starts[:, None] + phase_frac[None, :] * p

        if query_x.min() < x[0] or query_x.max() > x[-1]:
            continue

        mat_uw = np.interp(query_x.ravel(), x, u).reshape(n_full, nph)
        trend = slope * query_x + intercept
        errors = mat_uw - trend
        score = np.mean(np.var(errors, axis=0, ddof=1))

        if score < best_score:
            best_score, best_p = score, p

    return best_p, slope


# ============================================================
# Определение границ полных периодов
# ============================================================
def find_period_boundaries(x: np.ndarray, slope: float, intercept: float) -> np.ndarray:
    """
    Находит x-координаты границ периодов (где u = N × 360°).
    Возвращает только те границы, которые строго внутри данных.
    """
    u_start = slope * x[0] + intercept
    u_end = slope * x[-1] + intercept

    if slope > 0:
        first_n = int(np.ceil(u_start / 360.0))
        last_n = int(np.floor(u_end / 360.0))
        if first_n > last_n:
            return np.array([])
        boundary_ns = np.arange(first_n, last_n + 1)
    else:
        first_n = int(np.floor(u_start / 360.0))
        last_n = int(np.ceil(u_end / 360.0))
        if first_n < last_n:
            return np.array([])
        boundary_ns = np.arange(first_n, last_n - 1, -1)

    boundary_x = (boundary_ns * 360.0 - intercept) / slope

    eps = (x[-1] - x[0]) * 1e-6
    mask = (boundary_x >= x[0] + eps) & (boundary_x <= x[-1] - eps)
    boundary_x = boundary_x[mask]

    return np.sort(boundary_x)


# ============================================================
# Отсечение выбросных периодов (целиком)
# ============================================================
def filter_outlier_periods(error_mat: np.ndarray, clip: float = 3.0) -> np.ndarray:
    """
    Определяет, какие периоды являются выбросами.
    Критерий: RMS ошибки периода отличается от медианы более чем на clip*MAD.

    Возвращает маску: True = период OK, False = выброс.
    """
    n_periods = error_mat.shape[0]
    if n_periods < 3:
        return np.ones(n_periods, dtype=bool)

    # RMS ошибки для каждого периода
    rms_per_period = np.sqrt(np.mean(error_mat ** 2, axis=1))

    med = np.median(rms_per_period)
    mad = np.median(np.abs(rms_per_period - med))
    sigma = max(1.4826 * mad, 1e-12)

    mask = np.abs(rms_per_period - med) <= clip * sigma

    # Гарантируем минимум 2 периода
    if np.count_nonzero(mask) < 2:
        return np.ones(n_periods, dtype=bool)

    return mask


# ============================================================
# Fold полных периодов + глобальный детренд + статистика
# ============================================================
def fold_and_detrend(x: np.ndarray, angle: np.ndarray, std: np.ndarray,
                     period: float, slope_hint: float,
                     n_phase: int | None = None,
                     clip: float = 3.0,
                     filter_periods: bool = True) -> PeriodAnalysisResult:
    """
    Основная функция анализа:
      1. Находит полные периоды (неполные отбрасываются).
      2. Интерполирует каждый полный период на единую фазовую сетку.
      3. Вычитает ГЛОБАЛЬНЫЙ линейный тренд.
      4. Опционально отсекает выбросные периоды целиком.
      5. Считает mean и std по периодам поточечно.
    """
    dx = np.median(np.diff(x))
    if n_phase is None:
        n_phase = max(16, int(round(period / dx)))

    u_global = unwrap_deg(angle)
    slope_val, intercept_val, _ = robust_polyfit1(x, u_global, clip=clip)
    slope_sign = np.sign(slope_val)

    # --- 1. Определяем границы полных периодов ---
    boundary_x = find_period_boundaries(x, slope_val, intercept_val)

    if len(boundary_x) < 2:
        raise ValueError(
            f"Недостаточно полных периодов. "
            f"Найдено границ: {len(boundary_x)}, нужно >= 2."
        )

    n_periods = len(boundary_x) - 1
    period_starts = boundary_x[:-1]
    period_ends = boundary_x[1:]

    actual_periods = np.diff(boundary_x)
    period_refined = np.median(actual_periods)

    print(f"  Границ периодов: {len(boundary_x)}")
    print(f"  Полных периодов: {n_periods}")
    print(f"  Длины периодов: {actual_periods.min():.4f} .. {actual_periods.max():.4f} "
          f"(медиана {period_refined:.4f})")

    # --- 2. Фазовая сетка (центры бинов) ---
    phase_frac = (np.arange(n_phase) + 0.5) / n_phase

    # --- 3. Интерполяция каждого полного периода ---
    angle_mat = np.empty((n_periods, n_phase))
    std_mat = np.empty((n_periods, n_phase))
    query_x_mat = np.empty((n_periods, n_phase))

    for k in range(n_periods):
        xs = period_starts[k]
        xe = period_ends[k]
        local_period = xe - xs

        query_x = xs + phase_frac * local_period
        query_x_mat[k] = query_x

        angle_mat[k] = np.interp(query_x, x, u_global)
        std_mat[k] = np.interp(query_x, x, std)

    # --- 4. Глобальный линейный детренд ---
    trend = slope_val * query_x_mat + intercept_val
    error_mat = angle_mat - trend

    # --- 5. Отсечение выбросных периодов (целиком) ---
    if filter_periods and n_periods >= 3:
        period_mask = filter_outlier_periods(error_mat, clip=clip)
        n_outliers = np.count_nonzero(~period_mask)
        if n_outliers > 0:
            print(f"  Отсечено периодов-выбросов: {n_outliers}")
    else:
        period_mask = np.ones(n_periods, dtype=bool)

    # --- 6. Простой расчёт mean и std по валидным периодам ---
    valid_errors = error_mat[period_mask]
    valid_stds = std_mat[period_mask]
    n_valid = valid_errors.shape[0]

    mean_err = np.mean(valid_errors, axis=0)

    if n_valid > 1:
        stat_std = np.std(valid_errors, axis=0, ddof=1)
    else:
        stat_std = np.zeros(n_phase)

    # СКО одиночного измерения (RMS по периодам)
    single_std = np.sqrt(np.mean(valid_stds ** 2, axis=0))
    total_std = np.sqrt(stat_std ** 2 + single_std ** 2)

    # --- 7. Фазовая ось в градусах ---
    phase_samples = phase_frac * period_refined
    phase_deg = phase_frac * 360.0

    # Для убывающего сигнала инвертируем ось
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

    return PeriodAnalysisResult(
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
        fitted_slope=slope_val,
        rotation_direction="increasing" if slope_sign >= 0 else "decreasing",
        active_range=(float(x[0]), float(x[-1])),
        period_boundaries=boundary_x,
    )


# ============================================================
# Полный анализ
# ============================================================
def analyze(csv_path: str, sep: str = ",",
            n_hint: float = 10.0,
            min_p: float | None = None,
            max_p: float | None = None,
            n_phase: int | None = None,
            clip: float = 3.0,
            filter_periods: bool = True) -> PeriodAnalysisResult:

    x, angle, std = read_csv(csv_path, sep=sep)
    print(f"Загружено точек: {len(x)}")

    x, angle, std = trim_constants(x, angle, std)
    print(f"После trim_constants: {len(x)} точек, x ∈ [{x[0]:.1f}, {x[-1]:.1f}]")

    period, slope = estimate_period(x, angle, n_hint=n_hint)
    print(f"Оценка периода: {period:.4f} отсч., slope = {slope:.6f}°/отсч")

    if min_p and period < min_p:
        print(f"  Корректировка: {period:.2f} → {min_p}")
        period = min_p
    if max_p and period > max_p:
        print(f"  Корректировка: {period:.2f} → {max_p}")
        period = max_p

    return fold_and_detrend(x, angle, std, period, slope,
                            n_phase=n_phase, clip=clip,
                            filter_periods=filter_periods)


# ============================================================
# Визуализация
# ============================================================
def plot_result(r: PeriodAnalysisResult, out_path: str | None = None):
    ph = r.phase_deg
    fig, axes = plt.subplots(3, 1, figsize=(12, 10),
                             sharex=True, constrained_layout=True)

    ax1, ax2, ax3 = axes

    # --- График 1: средняя ошибка ± СКО ---
    ax1.fill_between(ph,
                     r.mean_error - r.total_std,
                     r.mean_error + r.total_std,
                     color="tab:orange", alpha=0.2, label="± total СКО")
    ax1.fill_between(ph,
                     r.mean_error - r.statistical_std,
                     r.mean_error + r.statistical_std,
                     color="tab:green", alpha=0.3, label="± stat СКО")
    ax1.plot(ph, r.mean_error, "k", lw=2, label="Средняя ошибка")
    ax1.axhline(0, color="gray", lw=0.5, ls="--")
    ax1.set_ylabel("Ошибка, град")
    ax1.set_title(
        f"Период = {r.period_samples:.4f} отсч. | "
        f"N = {r.n_periods} периодов | {r.rotation_direction}"
    )
    ax1.legend(fontsize=8, loc="upper right")
    ax1.set_xlim(0, 360)
    ax1.set_ylim(-0.1, 0.15)
    ax1.grid(True, alpha=0.3)

    # --- График 2: СКО ---
    ax2.plot(ph, r.single_frame_std, color="tab:blue", label="инстр. СКО")
    ax2.plot(ph, r.statistical_std, color="tab:green", label="стат. СКО")
    ax2.plot(ph, r.total_std, color="tab:orange", label="полное СКО")
    ax2.set_ylabel("СКО, град")
    ax2.legend(fontsize=8, loc="upper right")
    ax2.set_xlim(0, 360)
    ax2.set_ylim(0, 0.1)
    ax2.grid(True, alpha=0.3)

    # --- График 3: overlay периодов ---
    n_show = min(r.n_periods, 20)
    for k in range(n_show):
        ax3.plot(ph, r.error_matrix[k], alpha=0.8, lw=0.5)
    ax3.plot(ph, r.mean_error, "k", lw=2, label="mean")
    ax3.axhline(0, color="gray", lw=0.5, ls="--")
    ax3.set_xlabel("Фаза, град")
    ax3.set_ylabel("Ошибка, град")
    ax3.set_title(f"Отдельные периоды")
    ax3.set_xlim(0, 360)
    ax3.set_ylim(-0.15, 0.15)
    ax3.set_xticks(np.arange(0, 361, 30))
    ax3.grid(True, alpha=0.3)

    if out_path:
        fig.savefig(out_path, dpi=150, bbox_inches="tight")
    return fig


# ============================================================
# Сохранение CSV
# ============================================================
def save_csv(r: PeriodAnalysisResult, path: str):
    pd.DataFrame({
        "phase_samples": r.phase_samples,
        "phase_deg": r.phase_deg,
        "mean_error_deg": r.mean_error,
        "stat_std_deg": r.statistical_std,
        "single_std_deg": r.single_frame_std,
        "total_std_deg": r.total_std,
    }).to_csv(path, index=False)


# ============================================================
# CLI
# ============================================================
def main():
    p = argparse.ArgumentParser(
        description="Анализ периодического сигнала абсолютного энкодера"
    )
    p.add_argument("csv", help="Входной CSV: index, angle, std")
    p.add_argument("--sep", default=",")
    p.add_argument("--n-hint", type=float, default=10.0)
    p.add_argument("--min-period", type=float, default=None)
    p.add_argument("--max-period", type=float, default=None)
    p.add_argument("--n-phase", type=int, default=None)
    p.add_argument("--clip", type=float, default=3.0)
    p.add_argument("--no-filter", action="store_true",
                   help="Не отсекать выбросные периоды")
    p.add_argument("--plot", default="encoder_profile.png")
    p.add_argument("--out-csv", default="encoder_profile.csv")
    p.add_argument("--no-show", action="store_true")
    a = p.parse_args()

    r = analyze(a.csv, sep=a.sep, n_hint=a.n_hint,
                min_p=a.min_period, max_p=a.max_period,
                n_phase=a.n_phase, clip=a.clip,
                filter_periods=not a.no_filter)

    print(f"\n{'='*50}")
    print(f"РЕЗУЛЬТАТ")
    print(f"{'='*50}")
    print(f"Период:          {r.period_samples:.4f} отсч.")
    print(f"Направление:     {r.rotation_direction}")
    print(f"Периодов:        {r.n_periods}")
    print(f"Фазовых точек:   {len(r.phase_deg)}")
    print(f"Max |ошибка|:    {np.max(np.abs(r.mean_error)):.6f}°")
    print(f"RMS ошибки:      {np.sqrt(np.mean(r.mean_error**2)):.6f}°")
    print(f"Медиана stat СКО:   {np.median(r.statistical_std):.6f}°")
    print(f"Медиана single СКО: {np.median(r.single_frame_std):.6f}°")
    print(f"{'='*50}")

    save_csv(r, a.out_csv)
    plot_result(r, out_path=a.plot)
    print(f"\nСохранено: {a.out_csv}, {a.plot}")

    if not a.no_show:
        plt.show()


if __name__ == "__main__":
    main()
