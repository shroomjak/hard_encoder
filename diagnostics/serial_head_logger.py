#!/usr/bin/env python3
"""Запись ESP32 DATA в diagnostics/<имя>/<имя>.csv.

python diagnostics/serial_head_logger.py --port COM5 --ids 2,7 --output run_01
Без --output имя запрашивается при запуске. CSV: seq,ang_2,ang_7.
"""
from __future__ import annotations

import argparse
import csv
import math
from pathlib import Path
import re
import sys
import time

DIAGNOSTICS = Path(__file__).resolve().parent
ANSI = re.compile(rb'\x1b\[[0-?]*[ -/]*[@-~]')
HEADER = re.compile(rb'\bDATA seq=(\d{1,5})\b')
ITEM = re.compile(rb' id=(\d{1,3}) ang=([+-]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?)')
MAX_LINE = 16384
READ_SIZE = 65536
FLUSH_SECONDS = 0.5


def parse_ids(text: str) -> tuple[int, ...]:
    try:
        ids = tuple(int(p) for p in text.split(','))
        if not ids or len(set(ids)) != len(ids) or any(not 1 <= i <= 247 for i in ids):
            raise ValueError
        return ids
    except ValueError:
        raise argparse.ArgumentTypeError('Укажите уникальные ID 1..247: 2,7') from None


def output_path(name: str) -> Path:
    name = name.strip()
    if not name or '/' in name or '\\' in name or name in ('.', '..'):
        raise ValueError('Укажите имя эксперимента без пути')
    stem = name[:-4] if name.lower().endswith('.csv') else name
    if not stem or stem in ('.', '..'):
        raise ValueError('Укажите непустое имя эксперимента')
    folder = DIAGNOSTICS / stem
    folder.mkdir(exist_ok=True)
    return folder / f'{stem}.csv'


def parse_data(line: bytes, ids: tuple[int, ...], selected: frozenset[int]):
    if b'DATA' not in line:
        return None
    line = ANSI.sub(b'', line).rstrip(b'\r\n')
    match = HEADER.search(line)
    if match is None:
        return None
    seq = int(match[1])
    if seq > 65535:
        return None
    angles = {}
    pos = match.end()
    while pos < len(line):
        item = ITEM.match(line, pos)
        if item is None:
            return None
        head = int(item[1])
        if head not in selected or head in angles:
            return None
        angle = float(item[2])
        if not math.isfinite(angle) or not 0 <= angle < 360:
            return None
        angles[head] = format(angle, '.6f')
        pos = item.end()
    if len(angles) != len(ids):
        return None
    return (seq, *(angles[i] for i in ids))


class LineBuffer:
    def __init__(self):
        self.pending = bytearray()
        self.discarding = False

    def feed(self, chunk: bytes):
        start = 0
        while start < len(chunk):
            end = chunk.find(b'\n', start)
            stop = len(chunk) if end < 0 else end + 1
            if not self.discarding:
                if len(self.pending) + stop - start > MAX_LINE:
                    self.pending.clear()
                    self.discarding = True
                else:
                    self.pending.extend(chunk[start:stop])
            if end >= 0:
                if not self.discarding:
                    yield bytes(self.pending)
                self.pending.clear()
                self.discarding = False
            start = stop


def build_parser():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--port', required=True, help='COM5 или /dev/ttyUSB0')
    parser.add_argument('--ids', required=True, type=parse_ids, help='ID головок и порядок столбцов: 2,7')
    parser.add_argument('--output', help='Имя CSV/эксперимента; иначе запрос при запуске')
    parser.add_argument('--baud', type=int, default=115200, help='Скорость консоли ПК (115200)')
    parser.add_argument('--no-poll', action='store_true', help='Только читать, не отправлять poll')
    return parser


def run(args):
    import serial

    if args.baud <= 0:
        raise ValueError('--baud должен быть положительным')
    path = output_path(args.output or input('Имя CSV: '))
    ids = args.ids
    selected = frozenset(ids)
    with path.open('x', encoding='ascii', newline='', buffering=65536) as output:
        writer = csv.writer(output, lineterminator='\n')
        writer.writerow(('seq', *(f'ang_{head}' for head in ids)))
        output.flush()
        try:
            with serial.Serial(args.port, args.baud, timeout=0.1, write_timeout=2) as port:
                buffer = LineBuffer()
                poll_at = time.monotonic() + 1.0
                poll_sent = args.no_poll
                last_flush = time.monotonic()
                while True:
                    if not poll_sent and time.monotonic() >= poll_at:
                        command = ('poll ' + ','.join(map(str, ids)) + '\r\n').encode('ascii')
                        if port.write(command) != len(command):
                            raise OSError('Команда poll передана не полностью')
                        poll_sent = True
                    chunk = port.read(min(max(port.in_waiting, 1), READ_SIZE))
                    for line in buffer.feed(chunk):
                        row = parse_data(line, ids, selected)
                        if row is not None:
                            writer.writerow(row)
                    now = time.monotonic()
                    if now - last_flush >= FLUSH_SECONDS:
                        output.flush()
                        last_flush = now
        finally:
            output.flush()


def main():
    args = build_parser().parse_args()
    try:
        run(args)
    except KeyboardInterrupt:
        return 0
    except (OSError, ValueError, ImportError, EOFError) as exc:
        print(f'Error: {exc}', file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
