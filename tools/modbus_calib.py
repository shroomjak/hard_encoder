#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Ручная самокалибровка головки hard_encoder по Modbus RTU (RS-485).

Утилита делает ровно то, что нужно оператору, который сидит над шиной:
  frames  — напечатать готовые RTU-кадры (для любого терминала/скрипта);
  map     — карта регистров и битов;
  cmd     — отправить одну команду (ключ + код);
  status  — один опрос живого статуса калибровки;
  watch   — периодический опрос: прогресс, флаги, предупреждения;
  run     — вся последовательность 0x14 → 0x13 → 0x11 → 0x12 с ожиданиями;
  dump    — выгрузить таблицу (ang_tab, buf_k, …) в CSV постранично;
  diff    — сравнить две выгруженные таблицы.

Карта регистров здесь — КОПИЯ Inc/modbus_calib.h; расхождение ловит
tests/calib_map_test.py. Смысл команд и процедура: docs/calibration_modbus.md.

Линия: 115200 8E1 (чётность обязательна — так задано в прошивке).
pyserial не является обязательной: режимы frames/map работают без неё.
"""
from __future__ import annotations

import argparse
import math
import struct
import sys
import time

# ------------------------------ карта регистров ------------------------------

KEY_UNLOCK = 0xCA1B

HOLDING = {  # имя -> адрес на проводе (FC06)
    "SNAP": 0,
    "CAL_CMD": 1,
    "CAL_KEY": 2,
    "CAL_CFG": 3,
    "CAL_PAGE": 4,
}
HOLDING_COUNT = 5

CFG_RELAX = 0x0001
CFG_KEEP = 0x0002

COMMANDS = {  # код -> (имя, описание, ожидаемый код состояния при работе)
    0x10: ("STOP", "остановить всё, откатить прерванное (ключа не требует)", 0x00),
    0x11: ("ANGTAB_RIGHT", "ang_tab, проход 1: угол должен РАСТИ", 0x10),
    0x12: ("ANGTAB_LEFT", "ang_tab, проход 2: угол должен УБЫВАТЬ", 0x30),
    0x13: ("OFFSET", "калибровка точки съёма offset", 0x50),
    0x14: ("BUF_K", "калибровка чувствительности пикселей buf_k", 0x70),
}

INPUT_FIRST = 11
# Порядок, имена и единицы обязаны совпадать с enum CAL_R_* в
# Inc/modbus_calib.h; расхождение ловит tests/calib_map_test.py.
# kind="pair_lo" — младшее слово пары: 32-битные поля идут
# «старшее слово, затем младшее», отдельно они не декодируются.
LIVE_REGS = [
    ("BLOCK_ID", "hex", "маркер блока калибровок (0xCA10)"),
    ("STATE", "state", "encoder_state — код этапа"),
    ("AUTO_CAL", "u16", "проход ang_tab: 0 нет, 1 «вправо», 2 «влево»"),
    ("RDY1", "u16", "прогресс прохода 1 ang_tab, 0…144"),
    ("RDY2", "u16", "прогресс прохода 2 ang_tab, 0…144"),
    ("OFFSET_PHASE", "u16", "фаза автомата offset_cal"),
    ("OFFSET_CUR", "u16", "текущая проба offset"),
    ("OFFSET_FOUND", "u16", "найденный offset"),
    ("OFFSET_X256", "px", "живой offset, 1/256 пикселя"),
    ("ANGLEK_PHASE", "u16", "фаза автомата angk_cal"),
    ("ANGLEK_CUR", "u16", "номер кандидата усиления buf_k"),
    ("PIXMINMAX", "i16", "pix_min_max, отсчёты АЦП (цель <100)"),
    ("MINMAX_HI", "sudeg", "min_max — размах остатка, знаковые микроградусы"),
    ("MINMAX_LO", "pair_lo", ""),
    ("REVS", "u16", "оборотов учтено в текущей пробе"),
    ("REV_RIGHT", "u16", "rev_right_cnt (младшее слово 32-битного счётчика)"),
    ("REV_LEFT", "u16", "rev_left_cnt (младшее слово 32-битного счётчика)"),
    ("SERR1", "u16", "serrcnt1 — неисправимые кадры"),
    ("SERR2", "u16", "serrcnt2 — сработавшие побитовые коррекции"),
    ("SECTOR", "i16", "текущий номер сектора"),
    ("ANGLE_HI", "udeg", "живой угол cur_ang_E, микроградусы"),
    ("ANGLE_LO", "pair_lo", ""),
    ("FLAGS", "flags", "битовые флаги (см. FLAGS ниже)"),
    ("DIAG", "diag", "предупреждения (к прошлому опросу)"),
    ("LAST_CMD", "hex8", "код последней команды"),
    ("LAST_RESULT", "result", "результат последней команды"),
    ("CMD_COUNT", "u16", "команд принято"),
    ("REJ_COUNT", "u16", "команд отклонено"),
    ("CYCLES_HI", "u32", "cycles_max, такты (бюджет кадра ~17000)"),
    ("CYCLES_LO", "pair_lo", ""),
    ("BACKLIGHT", "u16", "backlight_width_ticks, ШИМ подсветки"),
    ("TEMP", "temp", "температура кристалла, 0,1 °C"),
    ("PIX_AVG", "u16", "pix_dif_num_avg — измерений на сектор"),
    ("OFFSET_AVG", "u16", "offset_avg_num — оборотов на пробу"),
    ("SAMPLE_HI", "u32", "кадров АЦП обработано (живой счётчик)"),
    ("SAMPLE_LO", "pair_lo", ""),
]
WINDOW_FLOATS = 16
WINDOW_FIRST = INPUT_FIRST + len(LIVE_REGS)   # 11 + 36 = 47
WINDOW_COUNT = 2 * WINDOW_FLOATS              # 32 регистра
WINDOW_LAST = WINDOW_FIRST + WINDOW_COUNT - 1  # 78
INPUT_TOTAL = WINDOW_LAST + 1                 # 79

TABLES = {  # имя -> (id в CAL_PAGE[15:8], длина, единица)
    "ang_tab": (0, 144, "градусы"),
    "buf_k": (1, 128, "безразмерный"),
    "pix_dif_tab1": (2, 144, "пиксели"),
    "pix_dif_tab2": (3, 144, "пиксели"),
    "offset_minmax": (4, 128, "градусы"),
}

FLAGS_BITS = [
    (0, "CAL_PENDING", "start_calibrate ещё не съеден автоматом"),
    (1, "OFFSET_RUN", "идёт offset_cal"),
    (2, "ANGK_RUN", "идёт angk_cal"),
    (3, "REV_EN", "детектор перехода 0/360 взведён"),
    (4, "AGC_ON", "АРУ подсветки разрешена"),
    (5, "EEPROM_IDLE", "запись в EEPROM завершена"),
    (6, "ERROR", "errorflag: последний кадр бракован"),
    (7, "ARMED", "ключ взведён, ждёт ровно одну команду"),
    (8, "RELAX", "проверки условий запуска отключены"),
    (9, "KEEP", "откат offset/buf_k при 0x10 выключен"),
    (10, "ANG_TAB_RDY", "ang_tab принята из EEPROM (CRC16 сошлась)"),
    (11, "BUF_K_RDY", "buf_k принята из EEPROM (CRC16 сошлась)"),
    (12, "OFFSET_RDY", "offset принят из EEPROM (CRC16 сошлась)"),
    (13, "LASDAC_RDY", "lasdac принят из EEPROM (CRC16 сошлась)"),
    (14, "BUSY", "калибровка идёт: новые команды отклоняются"),
    (15, "WAIT_PASS2", "состояние 0x20: проход 1 готов, ждём 0x12"),
]
DIAG_BITS = [
    (0, "MOVING_RIGHT", "угол растёт за прошлый опрос"),
    (1, "MOVING_LEFT", "угол убывает за прошлый опрос"),
    (2, "WRONG_DIRECTION", "направление не соответствует текущему проходу"),
    (3, "ANGTAB_STALLED", "прогресс ang_tab не двигался"),
    (4, "OFFSET_STALLED", "фаза offset_cal не меняется"),
    (5, "ANGK_STALLED", "фаза angk_cal не меняется"),
    (6, "MINMAX_UP", "min_max вырос относительно прошлого опроса"),
    (7, "SERR_GROWING", "прибавляются serrcnt1 — кадры бракуются"),
    (8, "NO_REVS", "оборот не детектируется (скорость выше ~57 об/мин)"),
    (9, "EVERY_OTHER", "прогресс застрял на 72 — неверное направление"),
    (10, "NO_FRAMES", "между опросами не было ни одного кадра АЦП"),
]
STATE_NAMES = {
    0x00: "штатный режим (стоп)",
    0x10: "идёт ang_tab, проход 1",
    0x20: "проход 1 готов — ждём команду 0x12",
    0x30: "идёт ang_tab, проход 2",
    0x40: "ang_tab готова и записана",
    0x50: "идёт offset_cal",
    0x60: "offset готов и записан",
    0x70: "идёт angk_cal",
    0x80: "buf_k готова и записана",
}
RESULT_NAMES = {
    0: "принято",
    1: "нет такого регистра",
    2: "неизвестный код команды или таблицы",
    3: "не взведён ключ CAL_KEY",
    4: "уже идёт калибровка",
    5: "EEPROM ещё пишет предыдущий блок",
    6: "нарушен порядок или не выполнено условие",
}
# Процедура: код -> (что ждём сразу, терминальные состояния, подсказка)
SEQUENCE = [
    (0x14, (0x80,), "10…40 об/мин, любое направление, ~2,5…3,5 мин"),
    (0x13, (0x60,), "10…40 об/мин, любое направление, ~36 с…2 мин"),
    (0x11, (0x20,), "5…20 об/мин, УГОЛ РАСТЁТ (rev_right)"),
    (0x12, (0x40,), "5…20 об/мин, УГОЛ УБЫВАЕТ (rev_left)"),
]

EXCEPTIONS = {
    0x01: "не та функция (SLAVE_FAILURE)",
    0x02: "адрес регистра вне карты (ILLEGAL DATA ADDRESS)",
    0x03: "значение недопустимо (ILLEGAL DATA VALUE)",
    0x04: "отказ устройства — см. LAST_RESULT",
    0x06: "занято: идёт калибровка или запись EEPROM (SLAVE_BUSY)",
    0x08: "таймаут ответа ведомого",
}


# ---------------------------------- Modbus ----------------------------------

def crc16(data: bytes) -> bytes:
    """CRC16 Modbus (полином A001, инверсии на входе/выходе), младший байт вперёд."""
    crc = 0xFFFF
    for b in data:
        crc ^= b
        for _ in range(8):
            crc = (crc >> 1) ^ 0xA001 if crc & 1 else crc >> 1
    return bytes((crc & 0xFF, crc >> 8))


def frame_write(slave: int, addr: int, value: int) -> bytes:
    body = bytes((slave, 0x06, addr >> 8, addr & 0xFF, value >> 8, value & 0xFF))
    return body + crc16(body)


def frame_read(slave: int, addr: int, count: int) -> bytes:
    body = bytes((slave, 0x04, addr >> 8, addr & 0xFF, count >> 8, count & 0xFF))
    return body + crc16(body)


def hexs(data: bytes) -> str:
    return " ".join(f"{b:02X}" for b in data)


class BusError(Exception):
    pass


class Bus:
    """Полудуплексный RTU-мастер: кадр, ответ, пауза t3.5 между запросами."""

    def __init__(self, port, baud, parity, stop, timeout, retries):
        try:
            import serial  # pyserial
        except ImportError as e:  # pragma: no cover - зависит от окружения
            raise BusError("нужна pyserial: pip install pyserial") from e
        parity_map = {"even": "E", "none": "N", "odd": "O"}
        self.s = serial.Serial(port=port, baudrate=baud, bytesize=8,
                               parity=parity_map[parity], stopbits=stop,
                               timeout=timeout, write_timeout=timeout)
        self.timeout = timeout
        self.retries = max(1, retries)
        self.t35 = max(0.001, 3.5 * 11 / baud)  # 11 бит на символ

    def close(self):
        try:
            self.s.close()
        except Exception:
            pass

    def _transact(self, req: bytes) -> bytes:
        self.s.reset_input_buffer()
        self.s.reset_output_buffer()
        self.s.write(req)
        self.s.flush()
        time.sleep(self.t35)
        chunks = bytearray()
        deadline = time.monotonic() + self.timeout
        while time.monotonic() < deadline:
            n = self.s.in_waiting
            if n:
                chunks += self.s.read(n)
                # Ответ закончен, когда линия молчит дольше t3.5
                silence = time.monotonic() + self.t35
                while time.monotonic() < silence:
                    if self.s.in_waiting:
                        break
                    time.sleep(0.0002)
                if not self.s.in_waiting:
                    break
            else:
                time.sleep(0.0002)
        return bytes(chunks)

    def request(self, req: bytes, expect_len: int) -> bytes:
        last = None
        for attempt in range(self.retries):
            if attempt:
                time.sleep(max(0.01, 2 * self.t35))
            resp = self._transact(req)
            if not resp:
                last = "нет ответа (таймаут)"
                continue
            if len(resp) < 4:
                last = f"обрывок ответа: {hexs(resp)}"
                continue
            if crc16(resp[:-2]) != resp[-2:]:
                last = "CRC16 ответа не сходится"
                continue
            if resp[0] != req[0]:
                last = f"ответ от другого адреса ({resp[0]})"
                continue
            if resp[1] == (req[1] | 0x80):
                exc = resp[2]
                raise BusError(f"исключение 0x{exc:02X} — {EXCEPTIONS.get(exc, 'неизвестное')}")
            if expect_len and len(resp) != expect_len:
                last = f"длина ответа {len(resp)} вместо {expect_len}"
                continue
            return resp
        raise BusError(f"шина не отвечает: {last}")

    def write_reg(self, addr: int, value: int) -> None:
        # Успешный FC06 — эхо запроса (8 байт).
        self.request(frame_write(self.slave, addr, value), 8)

    def read_regs(self, addr: int, count: int) -> list[int]:
        resp = self.request(frame_read(self.slave, addr, count), 5 + 2 * count)
        body = resp[2:-2]
        if body[0] != 2 * count:
            raise BusError("неожиданный объём данных в ответе FC04")
        return [struct.unpack(">H", body[1 + 2 * i: 3 + 2 * i])[0] for i in range(count)]

    slave = 1


def signed16(v: int) -> int:
    return v - 0x10000 if v & 0x8000 else v


def signed32(hi: int, lo: int) -> int:
    v = (hi << 16) | lo
    return v - 0x100000000 if v & 0x80000000 else v


def decode_status(regs: list[int]) -> dict:
    """regs — 36 регистров, начиная с INPUT_FIRST; на выходе — человекочитаемо."""
    out: dict = {}
    i = 0
    while i < len(LIVE_REGS):
        name, kind, _ = LIVE_REGS[i]
        v = regs[i]
        if kind in ("u32", "sudeg", "udeg"):
            pair = (v << 16) | regs[i + 1]
            if kind == "u32":
                out[name] = pair
            elif kind == "sudeg":
                # знаковые микроградусы → угловые секунды для чтения с экрана
                out[name] = (pair - 0x100000000 if pair & 0x80000000 else pair) / 3.6e6
            else:
                out[name] = pair / 1e6
            i += 2
            continue
        if kind == "pair_lo":
            i += 1
            continue
        if kind == "i16":
            out[name] = signed16(v)
        elif kind == "temp":
            out[name] = signed16(v) / 10.0
        elif kind == "px":
            out[name] = v / 256.0
        else:
            out[name] = v
        i += 1
    out["STATE_TEXT"] = STATE_NAMES.get(out["STATE"], "неизвестный код")
    out["BLOCK_OK"] = out["BLOCK_ID"] == 0xCA10
    out["FLAGS_ON"] = [n for b, n, _ in FLAGS_BITS if out["FLAGS"] >> b & 1]
    out["DIAG_ON"] = [n for b, n, _ in DIAG_BITS if out["DIAG"] >> b & 1]
    out["LAST_RESULT_TEXT"] = RESULT_NAMES.get(out["LAST_RESULT"], "?")
    return out


def live_addr(name: str) -> int:
    """Адрес на проводе по имени регистра (для тестов и режимов)."""
    for i, (n, _, _) in enumerate(LIVE_REGS):
        if n == name:
            return INPUT_FIRST + i
    raise KeyError(name)


# --------------------------------- режимы -----------------------------------

def mode_map(_: argparse.Namespace) -> int:
    print("holding (FC06), адрес на проводе:")
    note = {"SNAP": "зафиксировать снимок кадра",
            "CAL_CMD": "код команды 0x10…0x14",
            "CAL_KEY": f"0x{KEY_UNLOCK:04X} — ровно одна команда",
            "CAL_CFG": "бит0 RELAX, бит1 KEEP",
            "CAL_PAGE": "[15:8]=таблица, [7:0]=индекс первого элемента"}
    for name, addr in HOLDING.items():
        print(f"  {name:<9} {addr:>3}   4x{400001 + addr}   {note[name]}")
    print(f"\nвходные регистры (FC04): 0…10 — снимок кадра, {INPUT_FIRST}… — "
          f"живой блок калибровки (не зависит от SNAP)")
    for i, (name, kind, desc) in enumerate(LIVE_REGS):
        addr = INPUT_FIRST + i
        if kind == "pair_lo":
            print(f"  {addr:>3} {name:<13} (младшее слово пары)")
        else:
            print(f"  {addr:>3} {name:<13} {desc}")
    print(f"  {WINDOW_FIRST:>3} {'WINDOW':<13} окно выгрузки: {WINDOW_FLOATS} float "
          f"= {WINDOW_COUNT} регистров, старшее слово первым")
    print(f"  всего входных регистров: {INPUT_TOTAL} (адреса 0…{INPUT_TOTAL - 1})")
    print("\nкоманды:")
    for code in sorted(COMMANDS):
        name, desc, state = COMMANDS[code]
        print(f"  0x{code:02X} {name:<13} {desc}  (STATE 0x{state:02X})")
    print("\nтаблицы для CAL_PAGE: " +
          ", ".join(f"{n}={i}" for n, (i, _, _) in TABLES.items()))
    for title, bits in (("FLAGS", FLAGS_BITS), ("DIAG", DIAG_BITS)):
        print(f"биты {title}:")
        for b, name, desc in bits:
            print(f"  {b:>2} {name:<13} {desc}")
    return 0


def mode_frames(a: argparse.Namespace) -> int:
    s = a.slave
    parity_letter = {"even": "E", "odd": "O", "none": "N"}[a.parity]
    print(f"Головка: slave {s}.  Линия: {a.baud} 8{parity_letter}{a.stop}.")
    print("Порядок для ОДНОЙ команды: кадр ключа, затем кадр команды.\n")
    print(f"1) Взвести ключ (FC06, holding {HOLDING['CAL_KEY']} := 0x{KEY_UNLOCK:04X})")
    print("   ", hexs(frame_write(s, HOLDING["CAL_KEY"], KEY_UNLOCK)))
    print(f"\n2) Отправить команду (FC06, holding {HOLDING['CAL_CMD']} := код команды)")
    for code in sorted(COMMANDS):
        name, desc, state = COMMANDS[code]
        print(f"   0x{code:02X} {name:<13} {desc}  -> STATE 0x{state:02X}"
              + ("   ← без ключа" if code == 0x10 else ""))
        print("      ", hexs(frame_write(s, HOLDING["CAL_CMD"], code)))
    print(f"\n3) Живой статус калибровки (FC04, input {INPUT_FIRST}, 36 регистров)")
    print("   ", hexs(frame_read(s, INPUT_FIRST, 36)))
    print(f"4) Только код состояния (FC04, input {INPUT_FIRST + 1}, 1 регистр)")
    print("   ", hexs(frame_read(s, INPUT_FIRST + 1, 1)))
    print(f"5) Снимок + живой блок разом (FC04, input 0, {INPUT_FIRST + 36} регистров)")
    print("   ", hexs(frame_read(s, 0, INPUT_FIRST + 36)))
    print(f"\n6) Зафиксировать снимок (FC06, holding {HOLDING['SNAP']} := seq)")
    print("   ", hexs(frame_write(s, HOLDING["SNAP"], 42)))
    print(f"\n7) Выгрузить ang_tab, страница 0 (FC06, holding {HOLDING['CAL_PAGE']} "
          f":= 0x{0:02X}00), затем FC04 input {WINDOW_FIRST}, 32")
    print("   ", hexs(frame_write(s, HOLDING["CAL_PAGE"], 0)))
    print("   ", hexs(frame_read(s, WINDOW_FIRST, 2 * WINDOW_FLOATS)))
    print(f"   … индексы 16, 32, … (шаг {WINDOW_FLOATS} элементов, "
          f"{-(-TABLES['ang_tab'][1] // WINDOW_FLOATS)} страниц на ang_tab)")
    print(f"\n8) Режим (FC06, holding {HOLDING['CAL_CFG']}): 0=проверки вкл, "
          f"1=RELAX, 2=KEEP")
    for v in (0, CFG_RELAX, CFG_KEEP):
        print(f"   0x{v:04X}", hexs(frame_write(s, HOLDING["CAL_CFG"], v)))
    print("\nКаждый кадр адресный. Вещание (адрес 0) для CAL_* не использовать:")
    print("его исполнят ВСЕ головки сразу, а подтверждения не будет.")
    return 0


def open_bus(a: argparse.Namespace) -> Bus:
    bus = Bus(a.port, a.baud, a.parity, a.stop, a.timeout, a.retries)
    bus.slave = a.slave
    return bus


def read_status(bus: Bus) -> dict:
    regs = bus.read_regs(INPUT_FIRST, 36)
    return decode_status(regs)


def send_cmd(bus: Bus, code: int) -> None:
    if code != 0x10:
        bus.write_reg(HOLDING["CAL_KEY"], KEY_UNLOCK)
    bus.write_reg(HOLDING["CAL_CMD"], code)


def print_status(s: dict, prefix: str = "") -> None:
    if prefix:
        print(prefix, end="")
    print(f"STATE=0x{s['STATE']:02X} ({s['STATE_TEXT']})")
    print(f"  прогресс:  RDY1={s['RDY1']}/144  RDY2={s['RDY2']}/144  "
          f"offset: фаза {s['OFFSET_PHASE']} проба {s['OFFSET_CUR']} "
          f"итог {s['OFFSET_FOUND']}  buf_k: фаза {s['ANGLEK_PHASE']}")
    print(f"  метрика:   min_max={s['MINMAX_HI']:.2f}″  "
          f"pix_min_max={s['PIXMINMAX']} (цель <100)  оборотов в пробе={s['REVS']}")
    print(f"  угол:      {s['ANGLE_HI']:.5f}°  сектор={s['SECTOR']}  "
          f"ошибки: SERR1={s['SERR1']} SERR2={s['SERR2']}  "
          f"кадров={s['SAMPLE_HI']}  cycles={s['CYCLES_HI']}")
    print(f"  команда:   0x{s['LAST_CMD']:02X} → {s['LAST_RESULT_TEXT']} "
          f"(принято {s['CMD_COUNT']}, отклонено {s['REJ_COUNT']})")
    print(f"  флаги:     {' '.join(s['FLAGS_ON']) or '—'}")
    if s["DIAG_ON"]:
        print(f"  ВНИМАНИЕ:  {' '.join(s['DIAG_ON'])}")


def mode_status(a: argparse.Namespace) -> int:
    bus = open_bus(a)
    try:
        s = read_status(bus)
        if not s["BLOCK_OK"]:
            print("BLOCK_ID не 0xCA10: в прошивке нет блока калибровки "
                  "(или адрес головки неверный).")
            return 2
        print_status(s)
    finally:
        bus.close()
    return 0


def mode_watch(a: argparse.Namespace) -> int:
    bus = open_bus(a)
    try:
        while True:
            try:
                print_status(read_status(bus), prefix=f"[{time.strftime('%H:%M:%S')}] ")
            except BusError as e:
                print(f"[{time.strftime('%H:%M:%S')}] шина: {e}")
            time.sleep(a.interval)
    except KeyboardInterrupt:
        print("\nостановлен просмотр (команды головке не отправлялись)")
    finally:
        bus.close()
    return 0


def mode_cmd(a: argparse.Namespace) -> int:
    code = int(a.cmd, 16) if isinstance(a.cmd, str) else a.cmd
    if code not in COMMANDS:
        print(f"неизвестный код команды 0x{code:02X}; допустимо 0x10…0x14",
              file=sys.stderr)
        return 2
    bus = open_bus(a)
    try:
        send_cmd(bus, code)
        name, desc, _ = COMMANDS[code]
        print(f"0x{code:02X} {name} принята: {desc}")
        print_status(read_status(bus), prefix="  ")
    except BusError as e:
        print(f"отказ: {e}", file=sys.stderr)
        return 1
    finally:
        bus.close()
    return 0


def wait_for(bus: Bus, targets: tuple[int, ...], running: tuple[int, ...],
             note: str, timeout: float, interval: float) -> dict:
    """Ждём терминальное состояние, попутно показывая прогресс."""
    deadline = time.monotonic() + timeout
    last = None
    while time.monotonic() < deadline:
        s = read_status(bus)
        if s["STATE"] != last:
            last = s["STATE"]
            print(f"  → {s['STATE_TEXT']}")
        if s["STATE"] in targets:
            return s
        if s["DIAG_ON"]:
            print(f"  ! {' '.join(s['DIAG_ON'])}  ({note})")
        time.sleep(interval)
    raise BusError(f"ожидание состояний {['0x%02X' % t for t in targets]} "
                   f"превысило {timeout:.0f} с")


def mode_run(a: argparse.Namespace) -> int:
    steps = SEQUENCE
    if a.only:
        code = int(a.only, 16)
        steps = [s for s in SEQUENCE if s[0] == code]
        if not steps:
            print("--only принимает 0x11, 0x12, 0x13 или 0x14", file=sys.stderr)
            return 2
    bus = open_bus(a)
    try:
        s = read_status(bus)
        if not s["BLOCK_OK"]:
            print("BLOCK_ID не 0xCA10 — стоп. Проверьте прошивку и адрес.",
                  file=sys.stderr)
            return 2
        print("Проверки перед стартом:")
        problems = []
        if s["FLAGS"].__class__ and (s["FLAGS"] >> 5 & 1) == 0:
            problems.append("EEPROM ещё пишет предыдущий блок — подождите")
        if s["SAMPLE_HI"] == 0:
            problems.append("кадров АЦП ещё не было: головка не видит диск?")
        for p in problems:
            print("  !", p)
        if problems and not a.yes:
            print("продолжить? Ctrl-C для отмены, Enter — дальше", end="")
            input()
        for code, targets, note in steps:
            name, desc, running = COMMANDS[code]
            print(f"\n=== 0x{code:02X} {name}: {desc}")
            print(f"    {note}")
            if not a.yes:
                print("    Enter — отправить, Ctrl-C — прервать", end="")
                input()
            send_cmd(bus, code)
            s = read_status(bus)
            if s["STATE"] != running:
                print(f"    ! команда не запустилась: STATE=0x{s['STATE']:02X}, "
                      f"результат: {s['LAST_RESULT_TEXT']}")
                break
            print(f"    пошла; ждём {'/'.join('0x%02X' % t for t in targets)}")
            timeout = 420.0 if code in (0x14, 0x13) else 120.0
            wait_for(bus, targets, (running,), note, timeout, max(0.2, a.interval))
            # Ждём, пока запись блока уйдёт в EEPROM (иначе следующая калибровка
            # упрётся в MB_CAL_ERR_EEPROM).
            while (read_status(bus)["FLAGS"] >> 5 & 1) == 0:
                time.sleep(0.1)
            print("    готово, EEPROM записан")
        else:
            send_cmd(bus, 0x10)
            print("\n=== 0x10 STOP отправлена")
        print_status(read_status(bus), prefix="итог: ")
    except KeyboardInterrupt:
        print("\nпрервано оператором — отправляю 0x10 (стоп + откат)")
        try:
            bus.write_reg(HOLDING["CAL_CMD"], 0x10)
        except BusError as e:
            print(f"стоп не прошёл: {e}", file=sys.stderr)
    finally:
        bus.close()
    return 0


def dump_table(bus: Bus, table: str) -> list[float]:
    tid, length, _ = TABLES[table]
    values: list[float] = []
    while len(values) < length:
        idx = len(values)
        bus.write_reg(HOLDING["CAL_PAGE"], (tid << 8) | idx)
        words = bus.read_regs(WINDOW_FIRST, 2 * WINDOW_FLOATS)
        for w in range(0, 2 * WINDOW_FLOATS, 2):
            if len(values) >= length:
                break
            bits = (words[w] << 16) | words[w + 1]
            values.append(struct.unpack(">f", struct.pack(">I", bits))[0])
    return values


def mode_dump(a: argparse.Namespace) -> int:
    if a.table not in TABLES:
        print(f"неизвестная таблица {a.table}", file=sys.stderr)
        return 2
    bus = open_bus(a)
    try:
        values = dump_table(bus, a.table)
    finally:
        bus.close()
    _, _, unit = TABLES[a.table]
    if a.csv:
        with open(a.csv, "w", encoding="utf-8") as f:
            f.write(f"# head {a.slave} {time.strftime('%Y-%m-%d %H:%M:%S')} "
                    f"{a.table} ({unit})\n")
            f.write("index,value\n")
            for i, v in enumerate(values):
                f.write(f"{i},{v!r}\n")
        print(f"{len(values)} значений {a.table} → {a.csv}")
    else:
        for i, v in enumerate(values):
            print(f"{i:>4} {v!r}")
    return 0


def read_csv(path: str) -> list[float]:
    values = []
    with open(path, encoding="utf-8") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#") or "," not in line:
                continue
            a, b = line.split(",", 1)
            try:
                float(a)
            except ValueError:
                continue
            values.append(float(b))
    return values


def mode_diff(a: argparse.Namespace) -> int:
    before, after = read_csv(a.before), read_csv(a.after)
    if len(before) != len(after):
        print(f"разная длина: {len(before)} против {len(after)}")
        return 2
    d = [abs(x - y) for x, y in zip(before, after)]
    changed = [i for i, v in enumerate(d) if v > 1e-9]
    print(f"элементов: {len(before)}, изменились: {len(changed)}")
    print(f"среднее |Δ| = {sum(d) / len(d):.6g}, максимум |Δ| = {max(d):.6g}")
    if d and max(d) > 0:
        i = d.index(max(d))
        print(f"худший элемент [{i}]: {before[i]!r} → {after[i]!r}")
    return 0


# ----------------------------------- CLI ------------------------------------

def add_common(parser: argparse.ArgumentParser, suppress: bool = False) -> None:
    d = (lambda v: argparse.SUPPRESS) if suppress else (lambda v: v)
    parser.add_argument("--slave", "-s", type=int, default=d(1),
                        help="адрес головки 1…247")
    parser.add_argument("--port", default=d("/dev/ttyUSB0"),
                        help="COM5, /dev/ttyUSB0, /dev/ttyACM0…")
    parser.add_argument("--baud", type=int, default=d(115200))
    parser.add_argument("--parity", choices=("even", "none", "odd"), default=d("even"))
    parser.add_argument("--stop", type=int, default=d(1))
    parser.add_argument("--timeout", type=float, default=d(0.3),
                        help="ожидание ответа, с")
    parser.add_argument("--retries", type=int, default=d(2))
    parser.add_argument("--interval", type=float, default=d(0.5),
                        help="период опроса статуса, с")


def bus_args(a: argparse.Namespace) -> None:
    if not (1 <= a.slave <= 247):
        raise SystemExit("адрес головки должен быть 1…247: калибровка по вещанию "
                         "(адрес 0) запрещена — её исполнят все головки сразу, "
                         "и ответа не будет")
    if a.parity != "even":
        print("! прошивка принимает только чётность (8E1) — "
              "смените --parity even", file=sys.stderr)
        raise SystemExit(2)


def build_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(description=__doc__.splitlines()[0],
                                formatter_class=argparse.RawDescriptionHelpFormatter)
    add_common(p)
    sub = p.add_subparsers(dest="mode", required=True)
    for name, help_text, func in (
        ("frames", "напечатать готовые кадры RTU для ручной отправки", mode_frames),
        ("map", "напечатать таблицу регистров калибровки", mode_map),
        ("cmd", "отправить одну команду (0x10…0x14)", mode_cmd),
        ("status", "один опрос живого статуса калибровки", mode_status),
        ("watch", "непрерывный опрос статуса", mode_watch),
        ("run", "полная последовательность с ожиданием состояний", mode_run),
        ("dump", "выгрузить таблицу в CSV", mode_dump),
        ("diff", "сравнить две выгруженные таблицы", mode_diff),
    ):
        sp = sub.add_parser(name, help=help_text)
        add_common(sp, suppress=True)
        sp.set_defaults(func=func)
    sub.choices["cmd"].add_argument("--cmd", required=True, help="0x10…0x14")
    sub.choices["run"].add_argument("--only", help="одна команда: 0x11/0x12/0x13/0x14")
    sub.choices["run"].add_argument("--yes", action="store_true",
                                    help="без пауз на подтверждение")
    sub.choices["dump"].add_argument("--table", choices=tuple(TABLES),
                                     default="ang_tab")
    sub.choices["dump"].add_argument("--csv", help="файл для записи CSV")
    sub.choices["diff"].add_argument("before")
    sub.choices["diff"].add_argument("after")
    return p


def main(argv: list[str] | None = None) -> int:
    a = build_parser().parse_args(argv)
    for name, default in (("slave", 1), ("port", "/dev/ttyUSB0"), ("baud", 115200),
                          ("parity", "even"), ("stop", 1), ("timeout", 0.3),
                          ("retries", 2), ("interval", 0.5)):
        if not hasattr(a, name):
            setattr(a, name, default)
    if a.mode in ("frames", "map"):
        return a.func(a)
    bus_args(a)
    return a.func(a)


if __name__ == "__main__":
    sys.exit(main())
