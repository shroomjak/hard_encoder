#!/usr/bin/env python3
"""Пооборотный профиль head_period_analysis.py не зависит от выбора оси x.

Синтетическая запись двух противоположных головок: равномерное вращение, медленный
дрейф скорости привода и известная ошибка 1 раз на оборот. Окно скользящего
среднего задаётся в отсчётах, поэтому:
  - профили, построенные по seq и по t_us, обязаны совпасть;
  - медленный дрейф вычитается, а гармоника 1x/об сохраняется без ослабления.
Нужны numpy и pandas (зависимости проекта); без них тест пропускается.
Запуск: python3 tests/period_analysis_test.py
"""
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]

TURNS = 24                # полных оборотов в записи
SAMPLES_PER_TURN = 500    # отсчётов на оборот (5 с при опросе 100 Гц)
HARMONIC_DEG = 0.010      # амплитуда ошибки 1x/об первой головки
DRIFT_DEG = 0.020         # амплитуда медленного дрейфа привода (период 9 оборотов)


def synthetic_log(np, pd):
    k = np.arange(TURNS * SAMPLES_PER_TURN + SAMPLES_PER_TURN // 3)
    shaft = 360.0 * (k + 0.37 * SAMPLES_PER_TURN) / SAMPLES_PER_TURN
    drift = DRIFT_DEG * np.sin(2.0 * np.pi * k / (9.0 * SAMPLES_PER_TURN))
    error = HARMONIC_DEG * np.sin(np.radians(shaft))
    return pd.DataFrame({
        'seq': 42 + k,
        't_us': 869783 + 10000 * k,
        'error': 0,
        'ang_1': np.mod(shaft + drift + error, 360.0),
        'ang_2': np.mod(shaft + drift - error + 180.0, 360.0),
    })


def profile(h, df, x_column):
    columns = h.angle_columns(df)
    x_ref, ang_ref, signals = h.build_signals(df, columns, x_column)
    crossings, direction, _, grid, xc, slope = h.build_reference_grid(x_ref, ang_ref)
    return h.analyze_aligned(signals['ang_1'], crossings, direction, grid, xc, slope, '')


def main():
    try:
        import numpy as np
        import pandas as pd
    except ImportError as exc:
        print(f'SKIP period analysis: {exc}')
        return 0
    sys.path.insert(0, str(ROOT / 'diagnostics'))
    import head_period_analysis as h

    df = synthetic_log(np, pd)
    by_seq = profile(h, df, 'seq')
    by_time = profile(h, df, 't_us')

    assert by_seq.n_periods == by_time.n_periods == TURNS - 1, by_seq.n_periods
    assert np.allclose(by_seq.mean, by_time.mean, rtol=0.0, atol=1e-6), \
        float(np.max(np.abs(by_seq.mean - by_time.mean)))
    assert np.allclose(by_seq.std, by_time.std, rtol=0.0, atol=1e-6)

    for name, p in (('seq', by_seq), ('t_us', by_time)):
        expected = HARMONIC_DEG * np.sin(np.radians(p.angle_deg))
        # Гармоника 1x/об сохранена, дрейф и постоянный уровень вычтены.
        assert np.max(np.abs(p.mean - expected)) < 0.1 * HARMONIC_DEG, \
            (name, float(np.max(np.abs(p.mean - expected))))
        assert float(np.nanmax(p.std)) < 0.1 * HARMONIC_DEG, (name, float(np.nanmax(p.std)))

    print('Period analysis: profile identical for --x seq / --x t_us, 1x/rev harmonic preserved OK')
    return 0


if __name__ == '__main__':
    sys.exit(main())
