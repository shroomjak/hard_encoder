#!/usr/bin/env python3
"""Минимальный CSV: seq,t_us,snap_us,poll_us,error и
ang_ID,raw_ang_ID,age_us_ID,read_us_ID,gap_us_ID.
error: 1=SNAP, 2=READ, 4=невалидный снимок; биты объединяются.
seq сохраняется как uint16 без расширения. Скорость ПК по умолчанию 921600.
"""
import argparse
import csv
import math
from pathlib import Path
import queue
import re
import threading
import time

GLOBAL = ('seq', 't_us', 'snap_us', 'poll_us', 'error')
HEAD = ('ang', 'raw_ang', 'age_us', 'read_us', 'gap_us')
ANSI = re.compile(r'\x1b\[[0-?]*[ -/]*[@-~]')


def output_path(name):
    base = Path(__file__).resolve().parent
    text = str(name).strip().strip('"')
    if not text:
        raise ValueError('Не указано имя эксперимента')
    p = Path(text.replace('\\', '/')).expanduser()
    if p.is_absolute():
        if p.is_dir() or not p.suffix:
            return p / (p.name + '.csv')
        return p.with_suffix('.csv')
    if p.parts and p.parts[0] == base.name:
        p = Path(*p.parts[1:])
    if not p.parts or any(part == '..' for part in p.parts):
        raise ValueError('Укажите имя эксперимента или абсолютный путь')
    if len(p.parts) == 1:
        stem = p.stem if p.suffix.lower() == '.csv' else p.name
        return base / stem / (stem + '.csv')
    target = base / p
    if target.is_dir() or not target.suffix:
        return target / (target.name + '.csv')
    return target.with_suffix('.csv')


def _fmt_angle_field(raw_str, row_error):
    angle = float(raw_str)
    if math.isnan(angle):
        if row_error == 0:
            return None
        return 'nan'
    if not math.isfinite(angle) or not 0 <= angle < 360:
        return None
    return format(angle, '.6f')


def parse_data(line, ids):
    line = ANSI.sub('', line).strip()
    at = line.find('DATA ')
    if at < 0:
        return None
    parts = re.split(r' id=(\d+) ', line[at + 5:])
    if len(parts) != 1 + 2 * len(ids):
        return None
    try:
        g = dict(t.split('=', 1) for t in parts[0].split())
        row = [int(g[k]) for k in GLOBAL]
        if not 0 <= row[0] <= 65535 or any(v < 0 for v in row[1:]):
            return None
        heads = {}
        for n in range(1, len(parts), 2):
            ident = int(parts[n])
            if ident in heads:
                return None
            h = dict(t.split('=', 1) for t in parts[n + 1].split())
            value = _fmt_angle_field(h['ang'], row[4])
            if value is None:
                return None
            raw_value = (
                _fmt_angle_field(h['raw_ang'], row[4])
                if 'raw_ang' in h else value
            )
            if raw_value is None:
                return None
            age_us = int(h.get('age_us', '0'))
            read_us, gap_us = int(h['read_us']), int(h['gap_us'])
            if read_us < 0 or gap_us < 0:
                return None
            heads[ident] = [value, raw_value, age_us, read_us, gap_us]
        if set(heads) != set(ids):
            return None
        for ident in ids:
            row.extend(heads[ident])
        return row
    except (ValueError, KeyError):
        return None


def read_stop(commands):
    while True:
        try:
            text = input().strip()
        except EOFError:
            return
        if text == 'stop':
            commands.put('stop')
            return


def run(args):
    import serial
    ids = tuple(int(v) for v in args.ids.split(','))
    if not ids or len(set(ids)) != len(ids) or any(i < 1 or i > 247 for i in ids):
        raise ValueError('Нужны уникальные ID в диапазоне 1..247')
    if args.period_us is not None and not (2000 <= args.period_us <= 1000000):
        raise ValueError('period-us должен лежать в диапазоне 2000..1000000 мкс')
    name = args.output or input('Имя эксперимента: ').strip()
    path = output_path(name)
    path.parent.mkdir(parents=True, exist_ok=True)
    report_path = path.with_suffix('.timing.txt')
    commands = queue.Queue()
    header = list(GLOBAL) + [f'{key}_{ident}' for ident in ids for key in HEAD]
    poll_cmd = 'poll ' + ','.join(map(str, ids))
    if args.period_us is not None:
        poll_cmd += f' {args.period_us}'
    with path.open('x', encoding='ascii', newline='', buffering=65536) as out, \
         report_path.open('x', encoding='utf-8') as report, \
         serial.Serial(args.port, args.baud, timeout=.1, write_timeout=2) as port:
        threading.Thread(target=read_stop, args=(commands,), daemon=True).start()
        writer = csv.writer(out, lineterminator='\n')
        writer.writerow(header)
        out.flush()
        pending = bytearray()
        poll_at = time.monotonic() + 1.
        started, stopping = args.no_poll, False
        last_flush = time.monotonic()
        print(f'CSV: {path}\nДля завершения: stop + Enter или Ctrl+C один раз.')
        while True:
            try:
                if not started and time.monotonic() >= poll_at:
                    port.write((poll_cmd + '\r\n').encode('ascii'))
                    started = True
                try:
                    commands.get_nowait()
                    if not stopping:
                        port.write(b'stop\r\n')
                        stopping = True
                except queue.Empty:
                    pass
                pending.extend(port.read(min(max(port.in_waiting, 1), 65536)))
                while b'\n' in pending:
                    raw, _, tail = pending.partition(b'\n')
                    pending = bytearray(tail)
                    line = ANSI.sub('', raw.decode('ascii', errors='replace')).strip()
                    row = parse_data(line, ids)
                    if row is not None:
                        writer.writerow(row)
                    if any(tag in line for tag in ('POLL_END ', 'TIMING ', 'POLL_REPORT_DONE')):
                        report.write(line + '\n')
                        print(line)
                    if 'POLL_REPORT_DONE' in line and stopping:
                        out.flush()
                        report.flush()
                        return
                if len(pending) > 262144:
                    raise ValueError('Строка serial длиннее 256 KiB')
                if time.monotonic() - last_flush >= .5:
                    out.flush()
                    report.flush()
                    last_flush = time.monotonic()
            except KeyboardInterrupt:
                if stopping:
                    return
                port.write(b'stop\r\n')
                stopping = True
                print('STOP отправлен; ожидается итоговая статистика...')


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--port', required=True)
    p.add_argument('--ids', required=True)
    p.add_argument('--output', help='Имя: diagnostics/<имя>/<имя>.csv; либо явный путь')
    p.add_argument('--baud', type=int, default=921600, help='Скорость UART ПК, не RS485')
    p.add_argument('--period-us', type=int, default=None,
                   help='Период аппаратного таймера SNAP в мкс (по умолчанию 10000 = 100 Гц)')
    p.add_argument('--no-poll', action='store_true')
    args = p.parse_args()
    try:
        run(args)
    except (OSError, ValueError, ImportError) as exc:
        p.exit(1, f'[ERROR] {exc}\n')


if __name__ == '__main__':
    main()
