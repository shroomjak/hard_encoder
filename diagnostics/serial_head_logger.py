#!/usr/bin/env python3
"""serial_head_logger.py — чтение serial-порта ESP32 master и запись показаний головок.

Парсит строки ESP_LOG, публикуемые poll_heads() в esp32_master/main/main.c:

    SNAP seq=<N>
    ACK id=<id> seq=<N>
    DATA id=<id> seq=<N> angle=<deg>.<frac> deg ...

Список id головок задаётся вручную при запуске (--ids). По каждому seq
формируется строка

    seq,ang_1,...,ang_k,status

где k — число заданных головок, status = произведение статусов головок
(1 если все ACK, иначе 0). Имя файла: log_<name>.csv (префикс log_).
"""

from __future__ import annotations

import argparse
import csv
import re
import signal
import sys
import time
from pathlib import Path
from typing import Dict, List, Optional, Set

import serial


_RE_SNAP = re.compile(r"\bSNAP\s+seq=(\d+)")
_RE_ACK = re.compile(r"\bACK\s+id=(\d+)\s+seq=(\d+)")
_RE_DATA = re.compile(r"\bDATA\s+id=(\d+)\s+seq=(\d+)\s+angle=([0-9]+(?:\.[0-9]+)?)\s*deg")


def _normalize_output_name(name: str) -> Path:
    name = name.strip()
    if not name:
        raise ValueError("Пустое имя файла")
    p = Path(name)
    stem = p.stem if p.suffix.lower() == ".csv" else p.name
    if not stem.startswith("log_"):
        stem = "log_" + stem
    return Path(stem + ".csv")


def _parse_ids(text: str) -> List[int]:
    ids: List[int] = []
    seen: Set[int] = set()
    for part in text.replace(";", ",").split(","):
        part = part.strip()
        if not part:
            continue
        try:
            val = int(part)
        except ValueError:
            raise ValueError(f"Некорректный id: {part!r}")
        if val < 1 or val > 247:
            raise ValueError(f"Id вне диапазона 1..247: {val}")
        if val in seen:
            raise ValueError(f"Повторяющийся id: {val}")
        seen.add(val)
        ids.append(val)
    if not ids:
        raise ValueError("Список id пуст")
    return ids


class SeqAggregator:
    def __init__(self, seq: int, ids: List[int]):
        self.seq = seq
        self.ids = ids
        self.acks: Set[int] = set()
        self.angles: Dict[int, float] = {}

    def add_ack(self, head_id: int, seq: int) -> None:
        if seq == self.seq and head_id in self.ids:
            self.acks.add(head_id)

    def add_data(self, head_id: int, seq: int, angle: float) -> None:
        if seq == self.seq and head_id in self.ids:
            self.angles[head_id] = angle


def run(args: argparse.Namespace) -> None:
    # Список id.
    ids_text = args.ids
    if not ids_text:
        try:
            ids_text = input("Введите список id головок (например 1,2): ").strip()
        except EOFError:
            ids_text = ""
    try:
        ids = _parse_ids(ids_text)
    except ValueError as exc:
        print(f"[ERROR] {exc}", file=sys.stderr)
        sys.exit(2)

    # Имя выходного файла.
    if args.output:
        output_path = Path(args.output)
    else:
        name = args.name
        if not name and args.name_pos:
            name = args.name_pos
        if not name:
            try:
                name = input("Введите название лог-файла (без префикса log_): ").strip()
            except EOFError:
                name = ""
        if not name:
            print("[ERROR] Не указано имя файла", file=sys.stderr)
            sys.exit(2)
        output_path = _normalize_output_name(name)

    print(f"[INFO] Головки (id): {ids}", flush=True)
    print(f"[INFO] Выходной файл: {output_path}", flush=True)

    file_new = not output_path.exists() or output_path.stat().st_size == 0

    current: Optional[SeqAggregator] = None
    rows_written = 0

    csv_file = open(output_path, "a", newline="", buffering=1)
    writer = csv.writer(csv_file)
    if file_new:
        writer.writerow(["seq"] + [f"ang_{i}" for i in range(1, len(ids) + 1)] + ["status"])
        csv_file.flush()

    _stop = [False]

    def _sigint(sig, frame):
        _stop[0] = True

    signal.signal(signal.SIGINT, _sigint)

    print(f"[INFO] Open {args.port} @ {args.baud} baud", flush=True)
    try:
        ser = serial.Serial(args.port, args.baud, timeout=1.0)
    except serial.SerialException as exc:
        print(f"[ERROR] {exc}", file=sys.stderr)
        csv_file.close()
        sys.exit(1)

    time.sleep(1.0)
    print("[INFO] Чтение. Ctrl-C для завершения.", flush=True)

    def _finalize(agg: SeqAggregator) -> None:
        nonlocal rows_written
        if agg is None:
            return
        status = 1 if all(hid in agg.acks for hid in ids) else 0
        values = []
        for hid in ids:
            if hid in agg.angles:
                values.append(f"{agg.angles[hid]:.6f}")
            else:
                values.append("")
        writer.writerow([agg.seq] + values + [status])
        csv_file.flush()
        rows_written += 1
        print(f"[OK] seq={agg.seq} status={status} angles={values}", flush=True)

    ser.write("poll 1,2\r\n".encode("utf-8"))
    try:
        while not _stop[0]:
            raw = ser.readline()
            if not raw:
                continue
            try:
                text = raw.decode("utf-8", errors="replace")
            except Exception:
                continue
            
            m_snap = _RE_SNAP.search(text)
            if m_snap:
                if current is not None:
                    _finalize(current)
                    current = None
                current = SeqAggregator(int(m_snap.group(1)), ids)
                continue

            if current is None:
                continue

            m_ack = _RE_ACK.search(text)
            if m_ack:
                current.add_ack(int(m_ack.group(1)), int(m_ack.group(2)))
                continue

            m_data = _RE_DATA.search(text)
            if m_data:
                hid = int(m_data.group(1))
                seq = int(m_data.group(2))
                angle = float(m_data.group(3))
                current.add_data(hid, seq, angle)
                print("[DATA]", hid, seq, angle, flush=True)
                continue

        if current is not None:
            _finalize(current)
            current = None

    finally:
        ser.close()
        csv_file.close()
        print(f"\n[INFO] Done. rows={rows_written}.", flush=True)


def build_argparser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(description="Чтение показаний головок с serial-порта ESP32 master")
    src = p.add_argument_group("Serial port")
    src.add_argument("--port", required=True, help="COM-порт или TTY")
    src.add_argument("--baud", type=int, default=115200, help="Скорость, бод")
    ids = p.add_argument_group("Heads")
    ids.add_argument("--ids", default=None, help="Список id головок, напр. 1,2")
    out = p.add_argument_group("Output")
    out.add_argument("--name", default=None, help="Имя лога (без префикса log_)")
    out.add_argument("name_pos", nargs="?", default=None, help="Имя лога (позиционный)")
    out.add_argument("--output", default=None, help="Полный путь/имя выходного CSV")
    return p


def main() -> None:
    args = build_argparser().parse_args()
    run(args)


if __name__ == "__main__":
    main()
