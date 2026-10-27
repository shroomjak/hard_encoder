#!/usr/bin/env python3
"""ESP32 DATA -> CSV: seq,ang_1,...,ang_k (IDs follow --ids order).
Requires: python -m pip install pyserial
Example: python serial_angle_logger.py --port COM5 --ids 7,1 --output angles.csv
seq is the original uint16 value, including wrap/restarts/repetitions.
Only complete, valid DATA frames containing exactly the selected IDs are saved.
No metadata files, timestamps, status columns or per-frame terminal output.
"""
from __future__ import annotations

import argparse
import csv
import math
from pathlib import Path
import re
import sys
import time

ANSI = re.compile(rb'\x1b\[[0-?]*[ -/]*[@-~]')
HEADER = re.compile(rb'\bDATA seq=(\d{1,5})\b')
ITEM = re.compile(rb' id=(\d{1,3}) ang=([+-]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?)')
MAX_LINE = 16384
READ_SIZE = 65536
FLUSH_SECONDS = 0.5


def parse_ids(text: str) -> tuple[int, ...]:
    try:
        parts = text.split(',')
        if any(not re.fullmatch(r'\s*[0-9]{1,3}\s*', p) for p in parts):
            raise ValueError
        ids = tuple(int(p) for p in parts)
        if not ids or len(set(ids)) != len(ids) or any(not 1 <= i <= 247 for i in ids):
            raise ValueError
        return ids
    except ValueError:
        raise argparse.ArgumentTypeError('IDs must be unique integers 1..247, e.g. 7,1') from None


def parse_data(line: bytes, ids: tuple[int, ...], selected: frozenset[int]):
    """Ignore log prefixes, reject malformed DATA and incomplete groups."""
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
    """Bounded newline framing; discard oversized lines through next LF."""
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
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('--port', required=True, help='COM5 or /dev/ttyUSB0')
    parser.add_argument('--ids', required=True, type=parse_ids, help='CSV column order, e.g. 7,1')
    parser.add_argument('--output', required=True, type=Path, help='New CSV file; existing files are never overwritten')
    parser.add_argument('--baud', type=int, default=115200, help='PC console baud, not Modbus baud (default: 115200)')
    parser.add_argument('--no-poll', action='store_true', help='Read only; do not send poll command')
    return parser


def run(args):
    import serial

    if args.baud <= 0:
        raise ValueError('--baud must be positive')
    ids = args.ids
    selected = frozenset(ids)
    # Exclusive creation prevents accidental overwrite and races.
    with args.output.open('x', encoding='ascii', newline='', buffering=65536) as output:
        writer = csv.writer(output, lineterminator='\n')
        writer.writerow(('seq', *(f'ang_{i + 1}' for i in range(len(ids)))))
        output.flush()
        try:
            with serial.Serial(args.port, args.baud, timeout=0.1, write_timeout=2) as port:
                buffer = LineBuffer()
                poll_at = time.monotonic() + 1.0
                poll_sent = args.no_poll
                last_flush = time.monotonic()
                while True:
                    now = time.monotonic()
                    if not poll_sent and now >= poll_at:
                        command = ('poll ' + ','.join(map(str, ids)) + '\r\n').encode('ascii')
                        if port.write(command) != len(command):
                            raise OSError('Incomplete poll command write')
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
    except (OSError, ValueError, ImportError) as exc:
        print(f'Error: {exc}', file=sys.stderr)
        return 1
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
