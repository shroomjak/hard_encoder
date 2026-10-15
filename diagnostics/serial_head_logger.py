#!/usr/bin/env python3
"""serial_head_logger.py — чтение serial-порта ESP32 master и запись показаний головок.

Парсит строки ESP_LOG, публикуемые poll_heads() в esp32_master/main/main.c:

    SNAP seq=<N>
    ACK id=<id> seq=<N>
    DATA id=<id> seq=<N> angle=<deg>.<frac> deg ...

и агрегирует их по номеру снимка seq. На выходе CSV вида

    seq,ang_1,...,ang_k,status

где k — число разных id головок, status = произведение статусов головок
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


# Регулярные выражения под формат ESP_LOGI в main.c.
_RE_SNAP = re.compile(r"\bSNAP\s+seq=(\d+)")
_RE_ACK = re.compile(r"\bACK\s+id=(\d+)\s+seq=(\d+)")
_RE_DATA = re.compile(r"\bDATA\s+id=(\d+)\s+seq=(\d+)\s+angle=(\d+)\.(\d+)\s*deg")
_RE_STATUS_FAIL = re.compile(r"\bSTATUS\s+id=(\d+)\s+seq=(\d+)")


def _normalize_output_name(name: str) -> Path:
    """Приводит введённое имя к виду log_<name>.csv (без двойного префикса)."""
    name = name.strip()
    if not name:
        raise ValueError("Пустое имя файла")
    p = Path(name)
    stem = p.stem if p.suffix.lower() == ".csv" else p.name
    if stem.startswith("log_"):
        stem = stem  # уже с префиксом
    else:
        stem = "log_" + stem
    return Path(stem + ".csv")


def _parse_angle(whole: str, frac: str) -> float:
    # frac в логе всегда 6 знаков (микроградусы), но на всякий случай дополняем.
    frac = (frac + "000000")[:6]
    return float(whole) + float(frac) / 1_000_000.0


class SeqAggregator:
    """Собирает данные одного seq до его завершения."""

    def __init__(self, seq: int):
        self.seq = seq
        self.acks: Set[int] = set()
        self.failed: Set[int] = set()
        self.angles: Dict[int, float] = {}

    def add_ack(self, head_id: int) -> None:
        self.acks.add(head_id)

    def add_fail(self, head_id: int) -> None:
        self.failed.add(head_id)

    def add_data(self, head_id: int, angle: float) -> None:
        self.angles[head_id] = angle

    def ids(self) -> Set[int]:
        return self.acks | self.failed | set(self.angles)


def run(args: argparse.Namespace) -> None:
    # Определяем имя выходного файла.
    if args.output:
        output_path = Path(args.output)
    else:
        name = args.name
        if not name:
            try:
                name = input("Введите название лог-файла (без префикса log_): ").strip()
            except EOFError:
                name = ""
        if not name:
            print("[ERROR] Не указано имя файла", file=sys.stderr)
            sys.exit(2)
        output_path = _normalize_output_name(name)

    # Если передано полное имя без нормализации (например log_x.csv) — оставляем.
    if args.output is None and not str(output_path).startswith("log_"):
        output_path = Path("log_" + str(output_path))

    print(f"[INFO] Выходной файл: {output_path}", flush=True)

    file_new = not output_path.exists() or output_path.stat().st_size == 0

    # Ожидаемые головки накапливаются по мере поступления данных.
    expected_ids: List[int] = []
    expected_set: Set[int] = set()

    def _register_id(hid: int) -> None:
        nonlocal expected_ids, expected_set
        if hid not in expected_set:
            expected_set.add(hid)
            expected_ids.append(hid)
            expected_ids.sort()

    # Буфер текущего seq.
    current: Optional[SeqAggregator] = None
    rows_written = 0

    csv_file = open(output_path, "a", newline="", buffering=1)
    writer = None

    def _finalize_seq(agg: SeqAggregator) -> None:
        nonlocal writer, file_new, rows_written
        if agg is None:
            return
        if not agg.ids():
            return
        for hid in agg.ids():
            _register_id(hid)

        # Если головки ещё не известны, регистрируем из текущего seq.
        if not expected_ids:
            for hid in sorted(agg.ids()):
                _register_id(hid)

        # Статус: все ожидаемые головки (или головки этого seq) дали ACK.
        ids_for_status = expected_ids if expected_ids else sorted(agg.ids())
        status = 1
        for hid in ids_for_status:
            if hid not in agg.acks:
                status = 0
                break
        # Если ожидаемых ещё нет, но ACK есть — считаем по фактическим.
        if not expected_ids:
            if agg.failed or len(agg.acks) != len(agg.ids()):
                status = 0

        # Заголовок пишем при первой финализации, когда известно k.
        if writer is None:
            k = len(expected_ids) if expected_ids else len(agg.ids())
            if k == 0:
                k = len(agg.ids())
            if not expected_ids:
                for hid in sorted(agg.ids()):
                    _register_id(hid)
                k = len(expected_ids)
            header = ["seq"] + [f"ang_{i}" for i in range(1, k + 1)] + ["status"]
            if file_new:
                writer = csv.writer(csv_file)
                writer.writerow(header)
                csv_file.flush()
            else:
                writer = csv.writer(csv_file)

        k = len(expected_ids) if expected_ids else len(agg.ids())
        if not expected_ids:
            for hid in sorted(agg.ids()):
                _register_id(hid)
            k = len(expected_ids)

        # Формируем строку углов в порядке expected_ids.
        angle_values = []
        for hid in expected_ids:
            if hid in agg.angles:
                angle_values.append(f"{agg.angles[hid]:.6f}")
            else:
                angle_values.append("")  # нет DATA (status 0 или ошибка READ)

        writer.writerow([agg.seq] + angle_values + [status])
        csv_file.flush()
        rows_written += 1
        print(f"[OK] seq={agg.seq} status={status} angles={angle_values}", flush=True)

    # Обработка Ctrl-C.
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

    try:
        while not _stop[0]:
            raw = ser.readline()
            if not raw:
                # Тайм-аут: финализируем текущий seq, если он завис.
                if current is not None:
                    # Не финализируем преждевременно при паузе между строками:
                    # ждём ещё немного; если данных нет, финализируем.
                    pass
                continue

            try:
                text = raw.decode("utf-8", errors="replace")
            except Exception:
                continue

            m_snap = _RE_SNAP.search(text)
            if m_snap:
                # Финализируем предыдущий seq.
                if current is not None:
                    _finalize_seq(current)
                    current = None
                current = SeqAggregator(int(m_snap.group(1)))
                continue

            if current is None:
                # Строка до первого SNAP — игнорируем.
                continue

            m_ack = _RE_ACK.search(text)
            if m_ack:
                hid = int(m_ack.group(1))
                seq = int(m_ack.group(2))
                if seq == current.seq:
                    current.add_ack(hid)
                continue

            m_fail = _RE_STATUS_FAIL.search(text)
            if m_fail:
                hid = int(m_fail.group(1))
                seq = int(m_fail.group(2))
                if seq == current.seq:
                    current.add_fail(hid)
                continue

            m_data = _RE_DATA.search(text)
            if m_data:
                hid = int(m_data.group(1))
                seq = int(m_data.group(2))
                if seq == current.seq:
                    angle = _parse_angle(m_data.group(3), m_data.group(4))
                    current.add_data(hid, angle)
                continue

        # Завершение по Ctrl-C.
        if current is not None:
            _finalize_seq(current)
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
    out = p.add_argument_group("Output")
    out.add_argument("--name", default=None, help="Имя лога (без префикса log_)")
    out.add_argument("name_pos", nargs="?", default=None, help="Имя лога (позиционный)")
    out.add_argument("--output", default=None, help="Полный путь/имя выходного CSV")
    return p


def main() -> None:
    args = build_argparser().parse_args()
    if args.name_pos and not args.name:
        args.name = args.name_pos
    run(args)


if __name__ == "__main__":
    main()
