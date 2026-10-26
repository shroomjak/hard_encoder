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
import json
import math
import re
import sys
import time
from pathlib import Path

ANSI = re.compile(r"\x1b\[[0-?]*[ -/]*[@-~]")
HEADER = re.compile(r"\bDATA\s+seq=(\d+)\b")
ITEM = re.compile(r"\s+id=(\d+)\s+ang=([+-]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?)")
BOOT = re.compile(r"(?:\bMB config:|\bESP-ROM:|\brst:0x|^ets\s)")


def _parse_ids(text):
    parts = text.replace(';', ',').split(',')
    if not parts or any(not re.fullmatch(r'\s*\d+\s*', p) for p in parts):
        raise ValueError('IDs must be a nonempty comma-separated list')
    ids = [int(p) for p in parts]
    if any(not 1 <= h <= 247 for h in ids) or len(set(ids)) != len(ids):
        raise ValueError('IDs must be unique and within 1..247')
    return ids


def _normalize_output_name(name):
    p = Path(name.strip())
    if not p.name:
        raise ValueError('Empty output name')
    stem = p.stem if p.suffix.lower() == '.csv' else p.name
    return p.with_name(('' if stem.startswith('log_') else 'log_') + stem + '.csv')


def parse_data_line(text):
    """Return (wire seq, angles), None for non-DATA, ValueError for bad DATA."""
    text = ANSI.sub('', text).strip()
    m = HEADER.search(text)
    if m is None:
        if re.search(r'\bDATA\b', text):
            raise ValueError('Unsupported or malformed DATA header')
        return None
    seq = int(m[1])
    if not 0 <= seq <= 65535:
        raise ValueError('seq outside uint16')
    pos, angles = m.end(), {}
    while text[pos:].strip():
        item = ITEM.match(text, pos)
        if item is None:
            raise ValueError('Malformed/truncated DATA element')
        hid, value = int(item[1]), float(item[2])
        if not 1 <= hid <= 247 or hid in angles:
            raise ValueError('Invalid/duplicate head ID')
        if not math.isfinite(value) or not 0 <= value < 360:
            raise ValueError('Invalid angle')
        angles[hid] = value
        pos = item.end()
    if not angles:
        raise ValueError('DATA has no heads')
    return seq, angles


class LineBuffer:
    """Keep fragments across serial timeouts; never parse a partial line."""
    def __init__(self, limit=65536):
        self.pending = bytearray()
        self.limit = limit
        self.discarding = False
        self.oversized = 0

    def feed(self, chunk):
        result = []
        for part in chunk.splitlines(keepends=True):
            ended = part.endswith(b'\n')
            if not self.discarding:
                self.pending.extend(part)
                if len(self.pending) > self.limit:
                    self.pending.clear()
                    self.discarding = True
                    self.oversized += 1
            if ended:
                if not self.discarding:
                    result.append(self.pending.decode('utf-8', errors='replace'))
                self.pending.clear()
                self.discarding = False
        return result


class SeqTracker:
    def __init__(self, extended=None, session=0):
        self.previous = None
        self.extended = extended
        self.session = session
        self.reset_pending = False

    def mark_restart(self):
        self.reset_pending = self.previous is not None

    def accept(self, raw):
        if self.previous is None:
            self.extended = raw if self.extended is None else self.extended + 1
        elif self.reset_pending:
            self.session += 1
            self.extended += 1
        else:
            delta = (raw - self.previous) & 0xffff
            if delta == 0:
                return None
            if delta >= 32768:
                self.session += 1
                self.extended += 1
            else:
                self.extended += delta
        self.previous = raw
        self.reset_pending = False
        return self.extended, self.session


def build_argparser():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--port', required=True)
    p.add_argument('--baud', type=int, default=115200, help='ESP32 console baud, not RS485 baud')
    p.add_argument('--ids', help='ID order in CSV, e.g. 7,1')
    p.add_argument('--name')
    p.add_argument('name_pos', nargs='?')
    p.add_argument('--output')
    p.add_argument('--append', action='store_true', help='Append only to matching schema/ID mapping')
    p.add_argument('--no-poll', action='store_true', help='Do not send poll command')
    p.add_argument('--startup-delay', type=float, default=1.0)
    p.add_argument('--flush-interval', type=float, default=0.5)
    return p


def run(args):
    import serial
    ids = _parse_ids(args.ids or input('Head IDs (e.g. 1,2): '))
    name = args.name or args.name_pos
    path = Path(args.output) if args.output else _normalize_output_name(name or input('Log name: '))
    if args.baud <= 0 or args.startup_delay < 0 or args.flush_interval <= 0:
        raise ValueError('Invalid baud/delay/flush interval')
    header = ['seq'] + [f'ang_{i+1}' for i in range(len(ids))] + ['status', 'seq_raw', 'session', 'host_time_s']
    meta_path = path.with_suffix('.meta.json')
    extended, session = None, 0
    existing = path.exists() and path.stat().st_size > 0
    if existing:
        if not args.append:
            raise ValueError('Output exists; choose another name or use --append')
        meta = json.loads(meta_path.read_text(encoding='utf-8'))
        if meta.get('ids') != ids:
            raise ValueError('Existing CSV has a different ID mapping')
        with path.open(encoding='utf-8-sig', newline='') as f:
            reader = csv.DictReader(f)
            if reader.fieldnames != header:
                raise ValueError('Existing CSV has a different schema')
            last = None
            for row in reader:
                last = row
            if last:
                extended, session = int(last['seq']), int(last['session']) + 1
    if not existing and meta_path.exists():
        raise ValueError('Metadata already exists without a populated CSV; choose another output')
    with serial.Serial(args.port, args.baud, timeout=0.2) as ser:
        with path.open('a' if args.append else 'x', encoding='utf-8', newline='') as f:
            writer = csv.writer(f)
            if not existing:
                writer.writerow(header)
                meta_path.write_text(json.dumps({
                    'schema': 2, 'ids': ids, 'angle_unit': 'degree',
                    'status': 'all requested heads present in a valid DATA line',
                    'seq': 'extended counter; gaps preserved within a session',
                    'host_time_s': 'host receive time since this logger invocation; not acquisition time',
                    'restart_detection': 'boot markers or uint16 modular delta >=32768; not infallible'
                }, ensure_ascii=False, indent=2), encoding='utf-8')
            tracker, buffer = SeqTracker(extended, session), LineBuffer()
            counters = dict(rows=0, invalid=0, duplicate=0, incomplete=0)
            start = last_report = last_flush = time.monotonic()
            poll_at = start + args.startup_delay
            poll_sent = args.no_poll
            try:
                while True:
                    now = time.monotonic()
                    if not poll_sent and now >= poll_at:
                        ser.write(('poll ' + ','.join(map(str, ids)) + '\r\n').encode('ascii'))
                        poll_sent = True
                    chunk = ser.read(min(max(ser.in_waiting, 1), 65536))
                    for text in buffer.feed(chunk):
                        if BOOT.search(ANSI.sub('', text)):
                            tracker.mark_restart()
                        try:
                            parsed = parse_data_line(text)
                            print(parsed)
                        except ValueError:
                            counters['invalid'] += 1
                            continue
                        if parsed is None:
                            continue
                        raw_seq, angles = parsed
                        accepted = tracker.accept(raw_seq)
                        if accepted is None:
                            counters['duplicate'] += 1
                            continue
                        seq, session = accepted
                        status = int(all(h in angles for h in ids))
                        values = [f'{angles[h]:.6f}' if h in angles else '' for h in ids]
                        writer.writerow([seq, *values, status, raw_seq, session, f'{time.monotonic()-start:.9f}'])
                        counters['rows'] += 1
                        counters['incomplete'] += not status
                    now = time.monotonic()
                    if now - last_flush >= args.flush_interval:
                        f.flush()
                        last_flush = now
                    if now - last_report >= 1:
                        print(f'{counters} oversized={buffer.oversized}', file=sys.stderr, flush=True)
                        last_report = now
            except KeyboardInterrupt:
                pass
            finally:
                f.flush()
                print(f'Done: {counters}; trailing_bytes={len(buffer.pending)}', file=sys.stderr)


def main():
    try:
        run(build_argparser().parse_args())
    except (OSError, ValueError, ImportError) as exc:
        print(f'[ERROR] {exc}', file=sys.stderr)
        raise SystemExit(1)


if __name__ == '__main__':
    main()