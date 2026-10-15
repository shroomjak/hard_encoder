#!/usr/bin/env python3
"""
serial_angle_logger.py — непрерывное считывание пикселей CCD-сенсора с
serial-порта, оценка абсолютного угла диска и запись в CSV.

Конвейер обработки (точно соответствует demo_opencv_br.run для --source=matrix):

    pixels  ──► restore_signal_1d(k)          [коррекция дисторсии]
            ──► detect_edges(br_config)        [фильтр Blais-Rioux]
            ──► recover_bits(er, zeros, [])    [_detect_full_width]
            ──► _estimate_angle_if_possible    [De Bruijn lookup]
            ──► CSV: row_index, mean_angle_deg, std_angle_deg

Формат входных строк с serial-порта:
    <row_index>,<px0>,<px1>,...,<pxN>\\n

Использование (из корня репозитория):
    python serial_angle_logger.py --port COM3 --baud 115200
    python serial_angle_logger.py --port /dev/ttyUSB0 --baud 460800 \\
        --output log.csv --n-bits 64 --bit-width 6 --distort-k 0.05
"""

from __future__ import annotations

import argparse
import csv
import signal
import sys
import time
from pathlib import Path
from typing import Optional

import numpy as np
import serial

from src.blais_rioux import BRConfig
from src.demo_opencv_br import (
    _estimate_angle_if_possible,
    _detect_full_width,
    matrix_packets,
)

# Зеркало констант из demo_opencv_br (переопределяются через CLI)
_SENSOR_CENTER_PIXEL: Optional[float] = None   # None → 0.5*(n-1)
_ANGLE_PER_SENSOR_PIXEL_DEG: Optional[float] = None  # None → авто из bit_width
_DISTORT_K: float = 0.05
_ENABLE_ANGLE_ESTIMATION: bool = True

# ---------------------------------------------------------------------------
# Парсинг строки с serial-порта
# ---------------------------------------------------------------------------

def _parse_serial_line(text: str) -> Optional[tuple[int, np.ndarray]]:
    """«row_index,px0,px1,...» → (row_index, pixels). None при ошибке."""
    try:
        parts = text.strip().split(",")
        if len(parts) < 3:
            return None
        return int(parts[0]), np.array(parts[1:], dtype=np.float32)
    except Exception:
        return None


# ---------------------------------------------------------------------------
# Основной цикл
# ---------------------------------------------------------------------------

def run(args: argparse.Namespace) -> None:
    distort_k = args.distort_k if args.distort_k is not None else _DISTORT_K

    br_config = BRConfig(
        filter_order=args.filter_order,
        peak_threshold_rel=args.threshold / 100.0,
        min_edge_distance_factor=args.min_dist / 100.0,
        bit_width_px=args.bit_width,
        smoothing_sigma=args.smoothing,
        distort_coeff=distort_k,
    )

    output_path = Path(args.output)
    file_new = not output_path.exists() or output_path.stat().st_size == 0
    csv_file = open(output_path, "a", newline="", buffering=1)  # line-buffered
    writer = csv.writer(csv_file)
    if file_new:
        writer.writerow(["row_index", "mean_angle_deg", "std_angle_deg"])
        csv_file.flush()

    # Ctrl-C → корректное завершение
    _stop = [False]
    def _sigint(sig, frame):
        _stop[0] = True
    signal.signal(signal.SIGINT, _sigint)

    print(f"[INFO] Open {args.port} @ {args.baud} baud", flush=True)
    try:
        ser = serial.Serial(args.port, args.baud, timeout=2.0)
    except serial.SerialException as exc:
        print(f"[ERROR] {exc}", file=sys.stderr)
        csv_file.close()
        sys.exit(1)

    time.sleep(1.0)
    print(f"[INFO] Write {output_path}. Ctrl-C to stop.", flush=True)

    n_ok = n_fail = 0

    try:
        for packet in matrix_packets(args):
            if _stop[0]:
                break

            det = None
            try:
                det = _detect_full_width(
                    packet.pixels,
                    np.zeros(args.n_bits, dtype=np.int32),
                    np.array([]),
                    br_config,
                )
            except Exception as exc:
                print(f"[WARN] detect: {exc}", file=sys.stderr)

            angle_est = _estimate_angle_if_possible(det)

            if angle_est is None or angle_est.mean_angle_deg is None:
                n_fail += 1
                print(f"[SKIP] row={packet.row_index} (fails={n_fail})",
                      flush=True)
                continue

            mean_deg = angle_est.mean_angle_deg
            std_deg = angle_est.std_angle_deg if angle_est.std_angle_deg is not None else float(
                "nan")

            writer.writerow(
                [packet.row_index, f"{mean_deg:.6f}", f"{std_deg:.6f}"])
            csv_file.flush()
            n_ok += 1
            print(
                f"[OK]   row={packet.row_index:6d}  "
                f"angle={mean_deg:8.3f}°  std={std_deg:.4f}°  "
                f"(wrote={n_ok})",
                flush=True,
            )

    finally:
        ser.close()
        csv_file.close()
        print(f"\n[INFO] Done. OK={n_ok}, fails={n_fail}.", flush=True)


# ---------------------------------------------------------------------------
# CLI — аргументы зеркалят demo_opencv_br.build_argparser для --source=matrix
# ---------------------------------------------------------------------------

def build_argparser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(
        description=(
            "Считывает пиксели CCD с serial-порта, запускает полный конвейер "
            "Blais-Rioux + De Bruijn и пишет углы в CSV."
        )
    )

    src = p.add_argument_group("Serial port")
    src.add_argument("--port", required=True,
                     help="COM-порт или TTY (COM3, /dev/ttyUSB0)")
    src.add_argument("--baud", type=int, default=1000000,
                     help="Скорость, бод")

    out = p.add_argument_group("Output")
    out.add_argument("--output", default="angles.csv", metavar="FILE",
                     help="CSV-файл вывода (по умолч. angles.csv)")

    sim = p.add_argument_group("Sensor")
    sim.add_argument("--n-bits", type=int, default=24,
                     help="Ожидаемое число бит (для true_bits=zeros; по умолч. 24)")

    br = p.add_argument_group("Blais-Rioux")
    br.add_argument("--filter-order", type=int, choices=[2, 4], default=2,
                    help="Порядок фильтра (2 или 4, по умолч. 2)")
    br.add_argument("--threshold", type=float, default=50.0,
                    help="Порог пиков BR, %% (по умолч. 50)")
    br.add_argument("--min-dist", type=float, default=30.0,
                    help="Мин. расстояние между краями, %% (по умолч. 30)")
    br.add_argument("--bit-width", type=float, default=6.5, metavar="PX",
                    help="Ширина бита, пикс. (по умолч. 6.5)")
    br.add_argument("--smoothing", type=float, default=0.2, metavar="SIGMA",
                    help="Сигма сглаживания (по умолч. 0.2)")

    cor = p.add_argument_group("Distortion")
    cor.add_argument("--distort-k", type=float, default=None,
                     help=f"Коэф. дисторсии (по умолч. {_DISTORT_K})")

    return p


if __name__ == "__main__":
    run(build_argparser().parse_args())