#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Калибровка головки hard_encoder по Modbus RTU: отправка команд и контроль хода.

  frames  готовые RTU-кадры (для любого терминала или другой программы)
  map     карта регистров
  status  один опрос состояния калибровки
  watch   состояние калибровки с заданным интервалом
  cmd     отправить одну команду (0x10…0x14)
  run     последовательность 0x14, 0x13, 0x11, 0x12, 0x10 с ожиданием этапов
  dump    выгрузить ang_tab или buf_k в CSV
  diff    сравнить две выгруженные таблицы

Линия: 115200 8E1. Адрес головки обязателен: адрес 0 (вещание) утилита не
использует, потому что на вещание не отвечаёт ни одна головка.
Карта регистров — копия Inc/modbus_calib.h; сверяет tests/calib_map_test.py.
Процедура: docs/calibration_modbus.md.

Из зависимостей только pyserial (нужна для режимов обращения к шине).
"""
from __future__ import annotations

import argparse
import struct
import sys
import time

CMD = {
    0x10: ("STOP", "остановить калибровку", 0x00),
    0x11: ("ANGTAB_RIGHT", "ang_tab, проход 1 — угол должен РАСТИ", 0x10),
    0x12: ("ANGTAB_LEFT", "ang_tab, проход 2 — угол должен УБЫВАТЬ", 0x30),
    0x13: ("OFFSET", "offset_cal — перебор точки съёма", 0x50),
    0x14: ("BUFK", "angk_cal — выравнивание пикселей", 0x70),
}
DONE = {  # код команды -> состояние «готово»
    0x11: (0x20, "проход 1 готов, ждём 0x12"),
    0x12: (0x40, "ang_tab применена и записана"),
    0x13: (0x60, "offset записан"),
    0x14: (0x80, "buf_k записана"),
}
# Порядок и имена — как в enum MB_R_* (Inc/modbus_calib.h).
REGS = [
    ("STATE", "state", "encoder_state — код этапа"),
    ("RDY1", "u16", "прогресс прохода 1 ang_tab, 0…144"),
    ("RDY2", "u16", "прогресс прохода 2 ang_tab, 0…144"),
    ("OFFSET_PHASE", "u16", "фаза offset_cal (0…5, 100, 101)"),
    ("OFFSET_CUR", "u16", "текущая проба offset"),
    ("OFFSET_FOUND", "u16", "итог перебора offset"),
    ("OFFSET_X256", "px", "offset × 256 (пиксели)"),
    ("ANGK_PHASE", "u16", "фаза angk_cal (0…8, 100, 101)"),
    ("PIX_MIN_MAX", "i16", "разброс пиков, отсчёты АЦП; цель <100"),
    ("MINMAX_HI", "sudeg", "min_max, знаковые микроградусы"),
    ("MINMAX_LO", "pair", ""),
    ("REVS", "u16", "оборотов учтено в текущей пробе"),
    ("REV_RIGHT", "u16", "оборотов «вправо»"),
    ("REV_LEFT", "u16", "оборотов «влево»"),
    ("SERR1", "u16", "кадров с неисправимой ошибкой"),
    ("ANGLE_HI", "udeg", "текущий угол cur_ang_E, микроградусы"),
    ("ANGLE_LO", "pair", ""),
    ("FRAMES_HI", "u32", "обработано кадров АЦП"),
    ("FRAMES_LO", "pair", ""),
    ("EEPROM", "bits", "биты готовности EEPROM (см. map)"),
]
FIRST = 11              # MB_CAL_FIRST — адрес первого регистра статуса
WINDOW = 31             # MB_CAL_WINDOW — адрес окна выгрузки таблиц
WINDOW_FLOATS = 16      # MB_CAL_WINDOW_FLOATS
TOTAL = 63              # MB_CAL_TOTAL — адресов 0…62 в FC04
EEPROM_BITS = [(0, "запись завершена"), (1, "ang_tab принята"),
               (2, "buf_k принята"), (3, "offset принят")]
TABLES = {"ang_tab": (0, 144, "градусы"), "buf_k": (1, 128, "безразмерный")}
HELP = {"map": "карта регистров", "frames": "готовые RTU-кадры",
        "status": "один опрос состояния", "watch": "состояние с интервалом",
        "cmd": "одна команда", "run": "последовательность команд",
        "dump": "выгрузка таблицы в CSV", "diff": "сравнить две выгрузки"}
EXC = {0x01: "отказ устройства", 0x02: "адрес регистра вне карты",
       0x03: "недопустимое значение", 0x04: "нет такого кода команды или таблицы",
       0x06: "занято"}


# ------------------------------- Modbus RTU --------------------------------

def crc16(data: bytes) -> bytes:
    crc = 0xFFFF
    for b in data:
        crc ^= b
        for _ in range(8):
            crc = (crc >> 1) ^ 0xA001 if crc & 1 else crc >> 1
    return bytes((crc & 0xFF, crc >> 8))


def write_frame(slave: int, reg: int, value: int) -> bytes:
    body = bytes((slave, 0x06, reg >> 8, reg & 0xFF, value >> 8, value & 0xFF))
    return body + crc16(body)


def read_frame(slave: int, reg: int, count: int) -> bytes:
    body = bytes((slave, 0x04, reg >> 8, reg & 0xFF, count >> 8, count & 0xFF))
    return body + crc16(body)


def hexs(data: bytes) -> str:
    return " ".join(f"{b:02X}" for b in data)


class BusError(Exception):
    pass


class Bus:
    def __init__(self, port, baud, stop, timeout, retries):
        try:
            import serial
        except ImportError as e:
            raise BusError("не найдена pyserial: pip install pyserial") from e
        self.s = serial.Serial(port=port, baudrate=baud, bytesize=8, parity="E",
                               stopbits=stop, timeout=timeout, write_timeout=timeout)
        self.timeout, self.retries = timeout, max(1, retries)
        self.t35 = max(0.001, 3.5 * 11 / baud)

    def close(self):
        try:
            self.s.close()
        except Exception:
            pass

    def _ask(self, req: bytes) -> bytes:
        self.s.reset_input_buffer()
        self.s.reset_output_buffer()
        self.s.write(req)
        self.s.flush()
        time.sleep(self.t35)
        out = bytearray()
        end = time.monotonic() + self.timeout
        while time.monotonic() < end:
            n = self.s.in_waiting
            if n:
                out += self.s.read(n)
                quiet = time.monotonic() + self.t35
                while not self.s.in_waiting and time.monotonic() < quiet:
                    time.sleep(0.0002)
                if not self.s.in_waiting:
                    break
            else:
                time.sleep(0.0002)
        return bytes(out)

    def _transact(self, req: bytes, want_len: int) -> bytes:
        err = "нет ответа"
        for attempt in range(self.retries):
            time.sleep(0.02 if attempt else 0)
            resp = self._ask(req)
            if not resp:
                continue
            if len(resp) < 4 or crc16(resp[:-2]) != resp[-2:]:
                err = "CRC ответа не сходится"
                continue
            if resp[0] != req[0]:
                err = f"ответ от адреса {resp[0]}"
                continue
            if resp[1] == (req[1] | 0x80):
                raise BusError(f"исключение 0x{resp[2]:02X} ({EXC.get(resp[2], '?')})")
            if want_len and len(resp) != want_len:
                err = f"длина ответа {len(resp)}, ожидалось {want_len}"
                continue
            return resp
        raise BusError(err)

    def write(self, reg: int, value: int) -> None:
        self._transact(write_frame(self.slave, reg, value), 8)

    def read(self, reg: int, count: int) -> list[int]:
        resp = self._transact(read_frame(self.slave, reg, count), 5 + 2 * count)
        body = resp[2:-2]
        if body[0] != 2 * count:
            raise BusError("неверный объём данных в ответе")
        return [struct.unpack(">H", body[1 + 2 * i:3 + 2 * i])[0] for i in range(count)]

    slave = 1


def status(bus: Bus) -> dict:
    regs = bus.read(FIRST, len(REGS))
    out: dict = {}
    i = 0
    while i < len(REGS):
        name, kind, _ = REGS[i]
        v = regs[i]
        if kind in ("sudeg", "udeg", "u32"):
            pair = (v << 16) | regs[i + 1]
            key = name[:-3] if name.endswith("_HI") else name
            if kind == "sudeg":
                out[key] = (pair - 0x100000000 if pair & 0x80000000 else pair) / 3.6e6
            elif kind == "udeg":
                out[key] = pair / 1e6
            else:
                out[key] = pair
            i += 2
        else:
            out[name] = v - 0x10000 if kind == "i16" and v & 0x8000 else v
            i += 1
    out["EEPROM_BITS"] = [t for b, t in EEPROM_BITS if out["EEPROM"] >> b & 1]
    return out


def show(s: dict, prefix: str = "") -> None:
    print(f"{prefix}STATE=0x{s['STATE']:02X} — {state_text(s['STATE'])}")
    print(f"       ang_tab: проход 1 = {s['RDY1']}/144, проход 2 = {s['RDY2']}/144")
    print(f"       offset:  фаза {s['OFFSET_PHASE']}, проба {s['OFFSET_CUR']}, "
          f"итог {s['OFFSET_FOUND']}, текущий {s['OFFSET_X256'] / 256:.2f} пикс.")
    print(f"       buf_k:   фаза {s['ANGK_PHASE']}, разброс пиков {s['PIX_MIN_MAX']} "
          f"(цель <100)")
    print(f"       min_max = {s['MINMAX_HI']:.2f}″, оборотов в пробе {s['REVS']}, "
          f"вправо {s['REV_RIGHT']}, влево {s['REV_LEFT']}")
    print(f"       кадры: {s['FRAMES']}, брак {s['SERR1']}, угол {s['ANGLE']:.4f}°")
    print(f"       EEPROM: {', '.join(s['EEPROM_BITS']) or '—'}")


def state_text(code: int) -> str:
    return {0x00: "остановлено", 0x10: "идёт проход 1 ang_tab",
            0x20: "проход 1 готов — нужна команда 0x12",
            0x30: "идёт проход 2 ang_tab", 0x40: "ang_tab записана",
            0x50: "идёт offset_cal", 0x60: "offset записан",
            0x70: "идёт angk_cal", 0x80: "buf_k записана"}.get(code, "неизвестный код")


# ---------------------------------- режимы ---------------------------------

def mode_map(_: argparse.Namespace) -> int:
    print("Запись, FC06 (адреса на проводе):")
    print(f"   0  400001  SNAP — зафиксировать снимок кадра")
    print(f"   1  400002  CAL_CMD — код команды")
    print(f"   2  400003  CAL_PAGE — [15:8] таблица, [7:0] первый элемент")
    print("\nКоманды:")
    for c in sorted(CMD):
        name, desc, state = CMD[c]
        print(f"   0x{c:02X}  {name:<12} {desc}; сразу STATE=0x{state:02X}")
    for c in sorted(DONE):
        print(f"        готово после 0x{c:02X}: STATE=0x{DONE[c][0]:02X} — {DONE[c][1]}")
    print(f"\nЧтение, FC04 (0…10 — снимок кадра, {FIRST}… — калибровка):")
    for i, (name, kind, desc) in enumerate(REGS):
        print(f"   {FIRST + i:>3}  {name:<13} {desc}")
    print(f"   {WINDOW:>3}  {'WINDOW':<13} {WINDOW_FLOATS} float = "
          f"{2 * WINDOW_FLOATS} регистра, старшее слово первым")
    print(f"   всего: {TOTAL} (адреса 0…{TOTAL - 1})")
    print("\nEEPROM, биты:")
    for b, text in EEPROM_BITS:
        print(f"   {b}  {text}")
    print("\nТаблицы для CAL_PAGE: " + ", ".join(f"{n}={i}" for n, (i, _, _) in TABLES.items()))
    return 0


def mode_frames(a: argparse.Namespace) -> int:
    s = a.slave
    print(f"Головка {s}, линия 115200 8E1. Один кадр = одна команда.\n")
    print("Запуск калибровок (FC06, регистр 1):")
    for c in sorted(CMD):
        name, desc, state = CMD[c]
        print(f"   0x{c:02X} {name:<12} {desc:<45} {hexs(write_frame(s, 1, c))}")
    print(f"\nСостояние калибровки (FC04, регистр {FIRST}, {len(REGS)} шт):")
    print(f"   {hexs(read_frame(s, FIRST, len(REGS)))}")
    print(f"\nТолько STATE (FC04, регистр {FIRST}, 1 шт):")
    print(f"   {hexs(read_frame(s, FIRST, 1))}")
    print("\nВыгрузка ang_tab, страница 0 (FC06 регистр 2 := 0x0000, затем FC04 "
          f"регистр {WINDOW}, 32 шт):")
    print(f"   {hexs(write_frame(s, 2, 0x0000))}")
    print(f"   {hexs(read_frame(s, WINDOW, 2 * WINDOW_FLOATS))}")
    print(f"   следующие страницы: 0x0010, 0x0020, … ; всего "
          f"{-(-TABLES['ang_tab'][1] // WINDOW_FLOATS)} страниц")
    print("\nСнимок кадра (FC06, регистр 0 := 42) — отдельная команда:")
    print(f"   {hexs(write_frame(s, 0, 42))}")
    return 0


def mode_status(a: argparse.Namespace) -> int:
    bus = open_bus(a)
    try:
        show(status(bus))
    finally:
        bus.close()
    return 0


def mode_watch(a: argparse.Namespace) -> int:
    bus = open_bus(a)
    try:
        while True:
            try:
                show(status(bus), prefix=f"[{time.strftime('%H:%M:%S')}] ")
            except BusError as e:
                print(f"[{time.strftime('%H:%M:%S')}] {e}")
            time.sleep(a.interval)
    except KeyboardInterrupt:
        print("\nпросмотр прекращён")
    finally:
        bus.close()
    return 0


def mode_cmd(a: argparse.Namespace) -> int:
    code = int(a.cmd, 16)
    if code not in CMD:
        print("допустимые коды: 0x10, 0x11, 0x12, 0x13, 0x14", file=sys.stderr)
        return 2
    bus = open_bus(a)
    try:
        bus.write(1, code)
        print(f"команда 0x{code:02X} ({CMD[code][0]}) принята")
        show(status(bus))
    finally:
        bus.close()
    return 0


def wait_ready(bus: Bus, code: int, timeout: float, interval: float) -> None:
    state, note = DONE[code]
    running = CMD[code][2]
    deadline = time.monotonic() + timeout
    shown = None
    while True:
        s = status(bus)
        if s["STATE"] != shown:
            shown = s["STATE"]
            print(f"   STATE=0x{shown:02X} — {state_text(shown)}")
        if s["STATE"] == state:
            print(f"   {note}")
            return
        if time.monotonic() > deadline:
            raise BusError(f"ожидание STATE=0x{state:02X} превысило {timeout:.0f} с "
                           f"(прогресс: проход 1 = {s['RDY1']}, проход 2 = {s['RDY2']}, "
                           f"оборотов в пробе {s['REVS']})")
        time.sleep(interval)


def mode_run(a: argparse.Namespace) -> int:
    order = [0x14, 0x13, 0x11, 0x12]
    if a.only:
        only = int(a.only, 16)
        if only not in order:
            print("--only: 0x11, 0x12, 0x13 или 0x14", file=sys.stderr)
            return 2
        order = [only]
    bus = open_bus(a)
    try:
        for code in order:
            name, desc, running = CMD[code]
            print(f"\n0x{code:02X} {name}: {desc}")
            if code in (0x11, 0x12):
                s = status(bus)
                print(f"   обороты: вправо {s['REV_RIGHT']}, влево {s['REV_LEFT']} — "
                      f"проверьте направление вращения")
            if not a.yes:
                input("   Enter — отправить, Ctrl-C — выйти: ")
            bus.write(1, code)
            wait_ready(bus, code, 420.0 if code in (0x14, 0x13) else 120.0,
                       max(0.2, a.interval))
            while (status(bus)["EEPROM"] & 1) == 0:   # ждём конца записи
                time.sleep(0.1)
        if not a.only:
            bus.write(1, 0x10)
            print("\n0x10 STOP — калибровка остановлена, режим штатный")
        show(status(bus))
    except KeyboardInterrupt:
        print("\nпрервано — отправляю 0x10")
        try:
            bus.write(1, 0x10)
        except BusError as e:
            print(f"0x10 не прошла: {e}", file=sys.stderr)
    finally:
        bus.close()
    return 0


def dump_table(bus: Bus, table: str) -> list[float]:
    tid, length, _ = TABLES[table]
    values: list[float] = []
    while len(values) < length:
        bus.write(2, (tid << 8) | len(values))
        words = bus.read(WINDOW, 2 * WINDOW_FLOATS)
        for k in range(0, 2 * WINDOW_FLOATS, 2):
            if len(values) >= length:
                break
            values.append(struct.unpack(">f", struct.pack(
                ">I", (words[k] << 16) | words[k + 1]))[0])
    return values


def mode_dump(a: argparse.Namespace) -> int:
    bus = open_bus(a)
    try:
        values = dump_table(bus, a.table)
    finally:
        bus.close()
    if a.csv:
        with open(a.csv, "w", encoding="utf-8") as f:
            f.write(f"# головка {a.slave}, {time.strftime('%Y-%m-%d %H:%M:%S')}, "
                    f"{a.table} ({TABLES[a.table][2]})\n")
            f.write("index,value\n")
            for i, v in enumerate(values):
                f.write(f"{i},{v!r}\n")
        print(f"{a.table}: {len(values)} значений → {a.csv}")
    else:
        for i, v in enumerate(values):
            print(f"{i:>4} {v!r}")
    return 0


def read_csv(path: str) -> list[float]:
    out = []
    with open(path, encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            parts = line.split(",")
            if len(parts) == 2 and parts[0].isdigit():
                out.append(float(parts[1]))
    return out


def mode_diff(a: argparse.Namespace) -> int:
    b, t = read_csv(a.before), read_csv(a.after)
    if len(b) != len(t):
        print(f"разное число элементов: {len(b)} и {len(t)}")
        return 2
    d = [abs(x - y) for x, y in zip(b, t)]
    worst = max(range(len(d)), key=lambda i: d[i])
    print(f"элементов {len(b)}, изменились: {sum(1 for x in d if x > 1e-9)}")
    print(f"среднее |Δ| {sum(d) / len(d):.6g}, наибольшее |Δ| {d[worst]:.6g} "
          f"(элемент {worst}: {b[worst]!r} → {t[worst]!r})")
    return 0


def open_bus(a: argparse.Namespace) -> Bus:
    if not 1 <= a.slave <= 247:
        raise SystemExit("нужен адрес головки 1…247: на адрес 0 (вещание) головки не "
                         "отвечают, и команда ушла бы всем сразу")
    bus = Bus(a.port, a.baud, a.stop, a.timeout, a.retries)
    bus.slave = a.slave
    return bus


def add_common(p: argparse.ArgumentParser, sub: bool = False) -> None:
    keep = argparse.SUPPRESS if sub else None
    p.add_argument("-s", "--slave", type=int, default=keep if sub else 1,
                   help="адрес головки (1…247)")
    p.add_argument("--port", default=keep if sub else "/dev/ttyUSB0")
    p.add_argument("--baud", type=int, default=keep if sub else 115200)
    p.add_argument("--stop", type=int, default=keep if sub else 1)
    p.add_argument("--timeout", type=float, default=keep if sub else 0.3,
                   help="ожидание ответа, с")
    p.add_argument("--retries", type=int, default=keep if sub else 2)
    p.add_argument("--interval", type=float, default=keep if sub else 0.5,
                   help="период опроса в watch и run, с")


def main(argv=None) -> int:
    p = argparse.ArgumentParser(description=__doc__.strip().splitlines()[0],
                                formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = p.add_subparsers(dest="mode", required=True)
    add_common(p)
    for name, func in (("map", mode_map), ("frames", mode_frames),
                       ("status", mode_status), ("watch", mode_watch),
                       ("cmd", mode_cmd), ("run", mode_run),
                       ("dump", mode_dump), ("diff", mode_diff)):
        q = sub.add_parser(name, help=HELP[name])
        add_common(q, sub=True)
        q.set_defaults(func=func)
    sub.choices["cmd"].add_argument("--cmd", required=True, help="0x10…0x14")
    sub.choices["run"].add_argument("--only", help="одна команда: 0x11/0x12/0x13/0x14")
    sub.choices["run"].add_argument("--yes", action="store_true",
                                    help="без пауз на подтверждение")
    sub.choices["dump"].add_argument("--table", choices=tuple(TABLES), default="ang_tab")
    sub.choices["dump"].add_argument("--csv")
    sub.choices["diff"].add_argument("before")
    sub.choices["diff"].add_argument("after")

    a = p.parse_args(argv)
    for name, default in (("slave", 1), ("port", "/dev/ttyUSB0"), ("baud", 115200),
                          ("stop", 1), ("timeout", 0.3), ("retries", 2),
                          ("interval", 0.5)):
        if not hasattr(a, name):
            setattr(a, name, default)
    return a.func(a)


if __name__ == "__main__":
    sys.exit(main())
