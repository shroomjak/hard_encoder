#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Калибровка головки hard_encoder по Modbus RTU: отправка команд и контроль хода.

Линия: 115200, 8 данных, чётная чётность, 1 стоп-бит (8E1). Полудуплекс RS-485,
на одной линии может быть несколько головок — у каждой свой адрес Modbus
(MB_SLAVE_ADDRESS задаётся при сборке прошивки, см. Inc/modbus_board.h).

АДРЕС ГОЛОВКИ (slave id). Задаётся опцией -s / --slave (десятичное число 1…247)
и действует для всех режимов обращения к шине:

    python3 tools/modbus_calib.py -s 2 status
    python3 tools/modbus_calib.py --slave 2 run --port /dev/ttyUSB0
    python3 tools/modbus_calib.py -s 2 frames        # кадры для адреса 2

Адрес 0 — вещание: команды на нём выполняют все головки сразу, но ответа не будет
ни от одной, поэтому утилита его отвергает. Адрес на 247 тоже невозможен — это
зарезервированные значения спецификации Modbus. Если адрес неизвестен — режим
scan переберёт линию и покажет, кто отвечает:

    python3 tools/modbus_calib.py scan                          # адреса 1…247
    python3 tools/modbus_calib.py scan --from 1 --to 8         # только начало

Режимы (у каждого свои опции, подробности — `--help` режима):

  scan    перебрать адреса и показать ответившие головки
  status  один опрос состояния калибровки
  watch   состояние калибровки с заданным интервалом
  cmd     отправить одну команду (0x10…0x14)
  run     последовательность 0x14, 0x13, 0x11, 0x12, 0x10 с ожиданием этапов
  dump    выгрузить ang_tab или buf_k в CSV
  diff    сравнить две выгруженные таблицы (работает без шины)
  map     карта регистров (без шины)
  frames  готовые RTU-кадры для указанного -s (без шины)

Карта регистров — копия Inc/modbus_calib.h, сверяется тестом
tests/calib_map_test.py. Процедура: docs/calibration_modbus.md.
Из зависимостей только pyserial — нужна для режимов обращения к шине.
"""
from __future__ import annotations

import argparse
import struct
import sys
import time

# ---------------------------- коды и карта регистров ----------------------------
# Всё ниже — зеркало Inc/modbus_calib.h и Src/modbus_calib.c: адреса на проводе,
# коды команд и коды состояний. tests/calib_map_test.py сверяет их с прошивкой,
# поэтому менять здесь число или адрес можно только вместе с заголовком.

# Код команды в holding-регистре CAL_CMD: (имя, пояснение, STATE сразу после
# записи). Запись только взводит флаги запуска — сами калибровки ведут автоматы
# в Src/main.c, отсюда и отдельный STATE, и признак готовности ниже.
CMD = {
    0x10: ("STOP", "остановить калибровку", 0x00),
    0x11: ("ANGTAB_RIGHT", "ang_tab, проход 1 — угол должен РАСТИ", 0x10),
    0x12: ("ANGTAB_LEFT", "ang_tab, проход 2 — угол должен УБЫВАТЬ", 0x30),
    0x13: ("OFFSET", "offset_cal — перебор точки съёма", 0x50),
    0x14: ("BUFK", "angk_cal — выравнивание пикселей", 0x70),
}
# Порядок запуска — обратный нумерации кодов: он задан зависимостями в main.c
# (ang_tab измеряется в стробе от offset и по границам, выправленным buf_k; а
# min_max, который минимизируют offset_cal и angk_cal, считается по ang_tab).
RUN_ORDER = (0x14, 0x13, 0x11, 0x12)     # 0x10 — стоп, отдельно
# код: (скорость об/мин, ориентир по оборотам, верхняя граница ожидания, с).
# Ориентир в оборотах, потому что автоматы продвигаются по счётчику полных
# оборотов (avg_minmax_num), а не по времени.
RUN_INFO = {0x14: ("10…40", "50…70", 900.0),
            0x13: ("10…40", "12…68", 600.0),
            0x11: ("5…20", "5…10", 300.0),
            0x12: ("5…20", "5…10", 300.0)}
DONE = {  # код команды -> состояние «готово» и что оно значит
    0x11: (0x20, "проход 1 готов, ждём 0x12"),
    0x12: (0x40, "ang_tab применена и записана"),
    0x13: (0x60, "offset записан"),
    0x14: (0x80, "buf_k записана"),
}
# Живой блок входных регистров: (имя, вид, пояснение). Порядок и имена — как в
# enum MB_R_* (Inc/modbus_calib.h); вид задаёт разбор:
#   u16   — одно слово;  i16 — знаковое;  px — offset × 256;
#   *_HI/_LO — два слова (старшее первым): sudeg — знаковые микроградусы,
#   udeg — микроградусы, u32 — просто 32-битное число;  pair — младшее слово,
#   оно разбирается вместе с предыдущим;  bits — флаги EEPROM.
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
WINDOW_FLOATS = 16      # MB_CAL_WINDOW_FLOATS — float в окне
TOTAL = 63              # MB_CAL_TOTAL — адресов 0…62 в FC04
# Биты регистра EEPROM (MB_E_*): установленный бит — признак «всё в порядке».
EEPROM_BITS = [(0, "запись завершена"), (1, "ang_tab принята"),
               (2, "buf_k принята"), (3, "offset принят")]
# Таблицы, доступные для выгрузки: имя -> (номер для CAL_PAGE, элементов, единицы).
TABLES = {"ang_tab": (0, 144, "градусы"), "buf_k": (1, 128, "безразмерный")}
HELP = {"scan": "найти головки на линии", "map": "карта регистров",
        "frames": "готовые RTU-кадры", "status": "один опрос состояния",
        "watch": "состояние с интервалом", "cmd": "одна команда",
        "run": "последовательность команд", "dump": "выгрузка таблицы в CSV",
        "diff": "сравнить две выгрузки"}
# Коды исключений Modbus, которые умеет возвращать прошивка (см. mb_errors).
EXC = {0x01: "отказ устройства", 0x02: "адрес регистра вне карты",
       0x03: "недопустимое значение", 0x04: "нет такого кода команды или таблицы",
       0x06: "занято"}


# --------------------------------- Modbus RTU --------------------------------
# Кадр RTU: адрес_устройства, функция, данные, CRC16 (модель 8005, старший байт
# CRC идёт последним). FreeModbus на головке принимает FC06 (запись одного
# holding-регистра) и FC04 (чтение входных); FC03 и FC16 в сборке выключены.

def crc16(data: bytes) -> bytes:
    """CRC16-Modbus: те же 0xA001 и 0xFFFF, что в Src/main.c (crc_16_step)."""
    crc = 0xFFFF
    for b in data:
        crc ^= b
        for _ in range(8):
            crc = (crc >> 1) ^ 0xA001 if crc & 1 else crc >> 1
    return bytes((crc & 0xFF, crc >> 8))


def write_frame(slave: int, reg: int, value: int) -> bytes:
    """FC06: записать один holding-регистр reg на адресе slave."""
    body = bytes((slave, 0x06, reg >> 8, reg & 0xFF, value >> 8, value & 0xFF))
    return body + crc16(body)


def read_frame(slave: int, reg: int, count: int) -> bytes:
    """FC04: прочитать count входных регистров, начиная с reg."""
    body = bytes((slave, 0x04, reg >> 8, reg & 0xFF, count >> 8, count & 0xFF))
    return body + crc16(body)


def hexs(data: bytes) -> str:
    """Кадры в том виде, как их показывают терминалы: '01 06 …'."""
    return " ".join(f"{b:02X}" for b in data)


class BusError(Exception):
    """Нет ответа, битая CRC или чужой адрес — то есть линия не отвечает как надо."""


class BusException(BusError):
    """Головка ответила, но исключением Modbus. Значит, адрес на линии занят."""

    def __init__(self, code: int):
        # code — код исключения Modbus: 0x02 (нет такого регистра), 0x04
        # (недопустимое значение, у нас — код команды или номер таблицы), …
        super().__init__(f"исключение 0x{code:02X} ({EXC.get(code, '?')})")
        self.code = code


class Bus:
    """Порт RS-485 + обмен кадрами Modbus RTU с одним повтором по умолчанию.

    slave — адрес головки, выставляется в open_bus() из -s/--slave.
    timeout — сколько ждать начало ответа; retries — сколько попыток на кадр.
    """

    #: адрес головки на линии (1…247)
    slave = 1
    #: ожидание ответа при переборе адресов в scan, с
    probe_timeout = 0.08

    def __init__(self, port, baud, stop, timeout, retries):
        """Открыть порт; ошибки подключения здесь не ловятся — текст даёт pyserial."""
        try:
            import serial
        except ImportError as e:
            raise BusError("не найдена pyserial: pip install pyserial") from e
        # 8E1 — как в Inc/modbus_board.h и в проекте ESP32: без чётности головка
        # не поймёт кадр.
        self.s = serial.Serial(port=port, baudrate=baud, bytesize=8, parity="E",
                               stopbits=stop, timeout=timeout, write_timeout=timeout)
        self.timeout, self.retries = timeout, max(1, retries)
        # t3.5 — пауза, по которой Modbus отделяет кадр: 3.5 символа по 11 бит.
        self.t35 = max(0.001, 3.5 * 11 / baud)

    def close(self):
        """Закрыть порт; повторный close и порт, уже отданный системе, — не ошибка."""
        try:
            self.s.close()
        except Exception:
            pass

    def _ask(self, req: bytes) -> bytes:
        """Отправить кадр и собрать один ответ.

        Перед отправкой буферы вычищаются: лишний байт от предыдущего обмена
        сдвинул бы разбор всего ответа. После ответа ждём тишины t3.5 — по ней
        видно, что головка закончила передачу.
        """
        self.s.reset_input_buffer()
        self.s.reset_output_buffer()
        self.s.write(req)
        self.s.flush()
        time.sleep(self.t35)                     # смена направления DE/RE на головке
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

    def _transact(self, req: bytes, want_len: int, retries: int | None = None) -> bytes:
        """Обмен с проверками; retries — сколько попыток на этот кадр.

        Повторяется только то, что точно не дошло: тишина, мусор, неверная CRC.
        Ответ исключением повторяем не — головка кадр приняла и ответила.
        """
        err = "нет ответа"
        for attempt in range(self.retries if retries is None else retries):
            time.sleep(0.02 if attempt else 0)   # пауза перед повтором
            resp = self._ask(req)
            if not resp:
                continue
            if len(resp) < 4 or crc16(resp[:-2]) != resp[-2:]:
                err = "CRC ответа не сходится"
                continue
            if resp[0] != req[0]:
                # Ответ от чужого адреса: на линии два устройства с одним адресом
                # либо помеха. Считать такой ответ подтверждением нельзя.
                err = f"ответ от адреса {resp[0]}"
                continue
            if resp[1] == (req[1] | 0x80):
                raise BusException(resp[2])
            if want_len and len(resp) != want_len:
                err = f"длина ответа {len(resp)}, ожидалось {want_len}"
                continue
            return resp
        raise BusError(err)

    def write(self, reg: int, value: int) -> None:
        """FC06 с проверкой эха (ответ — тот же кадр), одна попытка.

        Повтор тут опасен: если головка команду выполнила, а эхо потерялось в
        линии, вторая отправка перезапустила бы проход калибровки с самого
        начала. Поэтому при сомнении лучше прочитать состояние и решить.
        """
        self._transact(write_frame(self.slave, reg, value), 8, retries=1)

    def read(self, reg: int, count: int) -> list[int]:
        """FC04: список из count слов, как их видит ProTool (старший байт первым)."""
        resp = self._transact(read_frame(self.slave, reg, count), 5 + 2 * count)
        body = resp[2:-2]
        if body[0] != 2 * count:
            raise BusError("неверный объём данных в ответе")
        return words(body)

    def probe(self, slave: int) -> tuple[list[int] | None, int | None]:
        """Один запрос статуса на адрес slave, для scan.

        Возвращает (слова, код исключения). (слова, None) — головка ответила;
        (None, None) — молчание; (None, код) — ответила исключением, адрес занят.
        Повторов нет, ожидание укороченное: иначе перебор 247 адресов идёт минуты.
        """
        req = read_frame(slave, FIRST, len(REGS))
        timeout, retries = self.timeout, self.retries
        self.timeout, self.retries = self.probe_timeout, 1
        try:
            resp = self._transact(req, 5 + 2 * len(REGS))
            return words(resp[2:-2]), None
        except BusException as e:
            return None, e.code
        except BusError:
            return None, None
        finally:
            self.timeout, self.retries = timeout, retries


def words(body: bytes) -> list[int]:
    """Разобрать блок данных FC04 на 16-битные слова."""
    return [struct.unpack(">H", body[1 + 2 * i:3 + 2 * i])[0] for i
            in range((len(body) - 1) // 2)]


# ------------------------------- разбор статуса --------------------------------

def decode(regs: list[int]) -> dict:
    """Слова из живого блока 11…30 -> понятные величины.

    32-битные поля приходят двумя словами (старшее первым) и складываются здесь;
    знак у PIX_MIN_MAX и MINMAX — по старшему разряду, как в прошивке.
    """
    out: dict = {}
    i = 0
    while i < len(REGS):
        name, kind, _ = REGS[i]
        v = regs[i]
        if kind in ("sudeg", "udeg", "u32"):
            pair = (v << 16) | regs[i + 1]     # старшее слово, как его кладёт прошивка
            key = name[:-3] if name.endswith("_HI") else name
            if kind == "sudeg":       # знаковые микроградусы -> угловые секунды
                signed = pair - 0x100000000 if pair & 0x80000000 else pair
                out[key] = signed * 3.6e-3
            elif kind == "udeg":       # микроградусы, делить на 1e6 — получаются градусы
                out[key] = pair / 1e6
            else:
                out[key] = pair
            i += 2
        else:
            out[name] = v - 0x10000 if kind == "i16" and v & 0x8000 else v
            i += 1
    out["EEPROM_BITS"] = [text for bit, text in EEPROM_BITS if out["EEPROM"] >> bit & 1]
    return out


def status(bus: Bus) -> dict:
    """Опросить живой блок и разобрать его."""
    return decode(bus.read(FIRST, len(REGS)))


def show(s: dict, prefix: str = "") -> None:
    """Человеческий вид состояния — то же, что §2.2 docs/calibration_modbus.md."""
    print(f"{prefix}STATE=0x{s['STATE']:02X} — {state_text(s['STATE'])}")
    print(f"       ang_tab: проход 1 = {s['RDY1']}/144, проход 2 = {s['RDY2']}/144")
    print(f"       offset:  фаза {s['OFFSET_PHASE']}, проба {s['OFFSET_CUR']}, "
          f"итог {s['OFFSET_FOUND']}, текущий {s['OFFSET_X256'] / 256:.2f} пикс.")
    print(f"       buf_k:   фаза {s['ANGK_PHASE']}, разброс пиков {s['PIX_MIN_MAX']} "
          f"(цель <100)")
    # Разрешение регистра — 1 микроградус = 0.001″, поэтому для min_max держим
    # три знака после запятой: меньше — теряем единицы последнего разряда.
    print(f"       min_max = {s['MINMAX']:.3f}″, оборотов в пробе {s['REVS']}, "
          f"вправо {s['REV_RIGHT']}, влево {s['REV_LEFT']}")
    print(f"       кадры: {s['FRAMES']}, брак {s['SERR1']}, угол {s['ANGLE']:.4f}°")
    print(f"       EEPROM: {', '.join(s['EEPROM_BITS']) or '—'}")


def state_text(code: int) -> str:
    """Расшифровка encoder_state (коды ставит Src/main.c)."""
    return {0x00: "остановлено", 0x10: "идёт проход 1 ang_tab",
            0x20: "проход 1 готов — нужна команда 0x12",
            0x30: "идёт проход 2 ang_tab", 0x40: "ang_tab записана",
            0x50: "идёт offset_cal", 0x60: "offset записан",
            0x70: "идёт angk_cal", 0x80: "buf_k записана"}.get(code, "неизвестный код")


# ----------------------------------- режимы ------------------------------------

def mode_scan(a: argparse.Namespace) -> int:
    """Перебрать адреса addr_from…addr_to и показать, кто на линии."""
    if a.addr_from < 1 or a.addr_to > 247 or a.addr_to < a.addr_from:
        print("диапазон адресов: 1…247, причём первый не больше последнего "
              f"(получено {a.addr_from}…{a.addr_to})", file=sys.stderr)
        return 2
    bus = Bus(a.port, a.baud, a.stop, a.timeout, 1)
    bus.probe_timeout = a.scan_timeout
    print(f"Перебор адресов {a.addr_from}…{a.addr_to} на {a.port} "
          f"({a.baud} 8E1), ожидание ответа {a.scan_timeout:.2f} с на адрес…")
    found = []
    try:
        for slave in range(a.addr_from, min(a.addr_to, 247) + 1):
            regs, exc = bus.probe(slave)
            if exc is not None:
                print(f"  адрес {slave}: отвечает исключением 0x{exc:02X} "
                      f"({EXC.get(exc, '?')}) — адрес занят")
                found.append(slave)
            elif regs is not None:
                s = decode(regs)
                print(f"  адрес {slave}: STATE=0x{s['STATE']:02X} "
                      f"({state_text(s['STATE'])}), угол {s['ANGLE']:.4f}°, "
                      f"кадров {s['FRAMES']}, EEPROM {s['EEPROM']}")
                found.append(slave)
    finally:
        bus.close()
    if not found:
        print("Никто не ответил. Проверить A/B, общий GND, DE/RE, скорость 115200 8E1 "
              "и что головка запитана. Адрес 0 (вещание) скан не проверяет: на него не "
              "отвечают.")
        return 1
    print("Найдено головок: " + str(len(found)) + " — " +
          ", ".join(str(x) for x in found))
    print('Дальше: python3 tools/modbus_calib.py -s %d status' % found[0])
    return 0


def mode_map(_: argparse.Namespace) -> int:
    """Карта регистров — то, что должно совпадать с Inc/modbus_calib.h."""
    print("Запись, FC06 (адреса на проводе):")
    print("   0  400001  SNAP — зафиксировать снимок кадра")
    print("   1  400002  CAL_CMD — код команды")
    print("   2  400003  CAL_PAGE — [15:8] таблица, [7:0] первый элемент")
    print("\nКоманды (порядок запуска: " +
          " → ".join(f"0x{c:02X}" for c in RUN_ORDER) + " → 0x10):")
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
    """Кадры для указанного -s: их можно отдать любому терминалу вместо утилиты."""
    check_slave(a.slave)
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
    """Один опрос состояния — его читают перед каждой командой и после неё."""
    bus = open_bus(a)
    try:
        show(status(bus), prefix=f"головка {a.slave}: ")
    finally:
        bus.close()
    return 0


def mode_watch(a: argparse.Namespace) -> int:
    """Циклический опрос: смотреть, как идёт калибровка, и ничего не ждать вручную."""
    bus = open_bus(a)
    try:
        while True:
            try:
                show(status(bus), prefix=f"[{time.strftime('%H:%M:%S')} id{a.slave}] ")
            except BusError as e:
                print(f"[{time.strftime('%H:%M:%S')}] {e}")
            time.sleep(a.interval)
    except KeyboardInterrupt:
        print("\nпросмотр прекращён")
    finally:
        bus.close()
    return 0


def mode_cmd(a: argparse.Namespace) -> int:
    """Одна команда и сразу состояние — удобно для проверки связи."""
    code = int(a.cmd, 16)
    if code not in CMD:
        print("допустимые коды: 0x10, 0x11, 0x12, 0x13, 0x14", file=sys.stderr)
        return 2
    bus = open_bus(a)
    try:
        bus.write(1, code)
        print(f"головка {a.slave}: команда 0x{code:02X} ({CMD[code][0]}) принята")
        show(status(bus))
    finally:
        bus.close()
    return 0


def wait_ready(bus: Bus, code: int, timeout: float, interval: float) -> None:
    """Ждать состояние готовности; при таймауте — сообщение с прогрессом."""
    state, note = DONE[code]
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


def run_steps(bus: Bus, a: argparse.Namespace, steps) -> None:
    """Отправить команды из steps, каждую — с ожиданием готовности и записью."""
    for code in steps:
        name, desc, _ = CMD[code]
        speed, revs, timeout = RUN_INFO[code]
        print(f"\n0x{code:02X} {name}: {desc}")
        print(f"   обороты {speed} об/мин, ориентир {revs} оборотов")
        s = status(bus)
        print(f"   до:     MINMAX={s['MINMAX']:.3f}″, PIX_MIN_MAX={s['PIX_MIN_MAX']}, "
              f"offset={s['OFFSET_X256'] / 256:.2f} пикс.")
        if code in (0x11, 0x12):
            print(f"   обороты: вправо {s['REV_RIGHT']}, влево {s['REV_LEFT']} — "
                  f"проверьте направление вращения")
        if not a.yes:
            input("   Enter — отправить, Ctrl-C — выйти: ")
        bus.write(1, code)
        wait_ready(bus, code, timeout, max(0.2, a.interval))
        while (status(bus)["EEPROM"] & 1) == 0:   # ждём конца записи в EEPROM
            time.sleep(0.1)
        s = status(bus)
        print(f"   после:  MINMAX={s['MINMAX']:.3f}″, PIX_MIN_MAX={s['PIX_MIN_MAX']}, "
              f"offset={s['OFFSET_X256'] / 256:.2f} пикс.")


def mode_run(a: argparse.Namespace) -> int:
    """Полная процедура: порядок, паузы на подтверждение, Ctrl-C = 0x10."""
    order = RUN_ORDER
    if a.only:
        only = int(a.only, 16)
        if only not in RUN_ORDER:
            print("--only: 0x11, 0x12, 0x13 или 0x14", file=sys.stderr)
            return 2
        order = (only,)
    bus = open_bus(a)
    print(f"Головка {a.slave}.")
    try:
        run_steps(bus, a, order)
        if not a.only:
            if a.twice:
                print("\nвторой круг: ang_tab уже записана, уточняем buf_k и offset")
                run_steps(bus, a, (0x14, 0x13))
            else:
                print("\nповторите 0x14 и 0x13 (--twice), если MINMAX после ang_tab не "
                      "улучшился: docs/calibration_modbus.md, шаг 9")
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
    """Выгрузить таблицу постранично: CAL_PAGE := номер_таблицы:индекс, FC04 окна.

    Головка держит в окне только 16 float, поэтому страниц несколько; индекс
    прошивка прижимает к концу таблицы, хвост за границей возвращает нули — его
    обрезаем по длине таблицы.
    """
    tid, length, _ = TABLES[table]
    values: list[float] = []
    while len(values) < length:
        bus.write(2, (tid << 8) | len(values))
        words_ = bus.read(WINDOW, 2 * WINDOW_FLOATS)
        for k in range(0, 2 * WINDOW_FLOATS, 2):
            if len(values) >= length:
                break
            values.append(struct.unpack(">f", struct.pack(
                ">I", (words_[k] << 16) | words_[k + 1]))[0])
    return values


def mode_dump(a: argparse.Namespace) -> int:
    """Выгрузка таблицы; с --csv — то, что потом сравнивается mode_diff-ом."""
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
    """Чтение выгрузки: строки 'номер,значение', комментарии с #."""
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
    """Сравнить две выгрузки: сколько элементов изменилось и насколько максимально."""
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


# --------------------------------- разбор аргументов --------------------------------

def check_slave(slave: int) -> None:
    """Проверка адреса головки — одна и для шины, и для режима frames.

    0 — вещание: команды на нём выполняют все головки сразу, ответа нет ни от
    одной, поэтому проверить результат невозможно. 248… — резерв спецификации.
    """
    if not 1 <= slave <= 247:
        raise SystemExit("нужен адрес головки 1…247 (-s/--slave): на адрес 0 (вещание) "
                         "головки не отвечают, а команда ушла бы всем сразу")


def open_bus(a: argparse.Namespace) -> Bus:
    """Открыть линию и назначить адрес головки из -s/--slave."""
    check_slave(a.slave)
    bus = Bus(a.port, a.baud, a.stop, a.timeout, a.retries)
    bus.slave = a.slave
    return bus


def add_common(p: argparse.ArgumentParser, sub: bool = False) -> int:
    """Общие опции. sub=True — внутри режима: тогда значение по умолчанию не
    перекрывает то, что уже разобрано на уровне программы (-s 2 run)."""
    keep = argparse.SUPPRESS if sub else 1
    p.add_argument("-s", "--slave", type=int, default=keep,
                   help="адрес головки на линии, 1…247 (0 — вещание — запрещён)")
    p.add_argument("--port", default=keep if sub else "/dev/ttyUSB0",
                   help="последовательный порт (COM5, /dev/ttyUSB0, /dev/ttyACM0)")
    p.add_argument("--baud", type=int, default=keep if sub else 115200)
    p.add_argument("--stop", type=int, default=keep if sub else 1,
                   help="стоповых битов (1 при чётности E)")
    p.add_argument("--timeout", type=float, default=keep if sub else 0.3,
                   help="ожидание ответа, с")
    p.add_argument("--retries", type=int, default=keep if sub else 2,
                   help="попыток на кадр чтения (запись всегда одна попытка)")
    p.add_argument("--interval", type=float, default=keep if sub else 0.5,
                   help="период опроса в watch и run, с")


def main(argv=None) -> int:
    """Сборка аргументов и вызов режима.

    Общие опции (-s, --port, …) принимаются и до имени режима, и после: до —
    потому что так привычнее, после — потому что у режима они тоже есть.
    Значение по умолчанию в subparser стоит argparse.SUPPRESS, чтобы режим не
    затирал то, что уже задано на уровне программы.
    """
    p = argparse.ArgumentParser(description=__doc__.strip().splitlines()[0],
                                formatter_class=argparse.RawDescriptionHelpFormatter,
                                epilog="Пример: python3 tools/modbus_calib.py "
                                       "-s 2 run --port /dev/ttyUSB0")
    sub = p.add_subparsers(dest="mode", required=True)
    add_common(p)
    for name, func in (("scan", mode_scan), ("map", mode_map),
                       ("frames", mode_frames), ("status", mode_status),
                       ("watch", mode_watch), ("cmd", mode_cmd), ("run", mode_run),
                       ("dump", mode_dump), ("diff", mode_diff)):
        q = sub.add_parser(name, help=HELP[name])
        add_common(q, sub=True)
        q.set_defaults(func=func)
    sub.choices["scan"].add_argument("--from", dest="addr_from", type=int, default=1,
                                     help="первый проверяемый адрес (1)")
    sub.choices["scan"].add_argument("--to", dest="addr_to", type=int, default=247,
                                     help="последний проверяемый адрес (247)")
    sub.choices["scan"].add_argument("--scan-timeout", type=float, default=0.08,
                                     help="ожидание на один адрес в scan, с")
    sub.choices["cmd"].add_argument("--cmd", required=True, help="0x10…0x14")
    sub.choices["run"].add_argument("--only", help="одна команда: 0x11/0x12/0x13/0x14")
    sub.choices["run"].add_argument("--yes", action="store_true",
                                    help="без пауз на подтверждение")
    sub.choices["run"].add_argument("--twice", action="store_true",
                                    help="после ang_tab повторить 0x14 и 0x13")
    sub.choices["dump"].add_argument("--table", choices=tuple(TABLES), default="ang_tab")
    sub.choices["dump"].add_argument("--csv")
    sub.choices["diff"].add_argument("before")
    sub.choices["diff"].add_argument("after")

    a = p.parse_args(argv)
    # Значения по умолчанию для опций, заданных до имени режима: subparser с
    # argparse.SUPPRESS их не трогает, а пользоваться ими надо во всех режимах.
    for name, default in (("slave", 1), ("port", "/dev/ttyUSB0"), ("baud", 115200),
                          ("stop", 1), ("timeout", 0.3), ("retries", 2),
                          ("interval", 0.5), ("scan_timeout", 0.08),
                          ("addr_from", 1), ("addr_to", 247)):
        if not hasattr(a, name):
            setattr(a, name, default)
    return a.func(a)


if __name__ == "__main__":
    try:
        sys.exit(main())
    except BusError as e:          # линия не отвечает, порт занят, нет pyserial —
        print(str(e), file=sys.stderr)     # одна строка вместо traceback-а
        sys.exit(1)
    except KeyboardInterrupt:              # Ctrl-C вне цикла опроса — не ошибка
        sys.exit(130)
