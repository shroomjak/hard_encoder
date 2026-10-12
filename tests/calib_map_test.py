#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Карта канала калибровки должна быть одна, а не три.

Сверяет Inc/modbus_calib.h, Src/modbus_calib.c, Src/modbus_registers.c,
Src/main.c, tests/host/calib_test.c, tools/modbus_calib.py, режим калибровки
esp32_master/main/calib_mode.c (и его Kconfig/CMake) и README.md:
адреса holding-регистров и окна, коды команд и состояния до/после, перечень и
порядок регистров статуса, биты EEPROM, идентификаторы и размеры таблиц.
Несовпадение — провал: расхождение карты в прошивке и в инструменте означает
команду, ушедшую в чужой регистр.
"""
from __future__ import annotations

import pathlib
import re
import sys

ROOT = pathlib.Path(__file__).resolve().parents[1]


def read(path: str, encoding: str = "utf-8") -> str:
    p = ROOT / path
    if not p.exists():
        print(f"нет файла {path}", file=sys.stderr)
        sys.exit(2)
    return p.read_bytes().decode(encoding)


def strip(text: str) -> str:
    """Без комментариев — чтобы искать по коду, а не по пояснениям."""
    text = re.sub(r"/\*.*?\*/", " ", text, flags=re.S)
    return re.sub(r"//[^\n]*", " ", text)


def defines(text: str) -> dict:
    """#define ИМЯ <число> (суффиксы u/U допускаются, комментарии убраны)."""
    out = {}
    for name, value in re.findall(r"#define\s+([A-Za-z_]\w*)[ \t]+([^\n]+)", text):
        m = re.fullmatch(r"(0[xX][0-9a-fA-F]+|\d+)[uUlL]*", value.strip())
        if m:
            out[name] = int(m.group(1), 0)
    return out


def enum_names(text: str) -> list:
    """Имена членов первого enum в файле, в порядке объявления."""
    m = re.search(r"enum\s*\w*\s*\{(.*?)\}", text, re.S)
    if m is None:
        return []
    out = []
    for item in m.group(1).split(","):
        mm = re.match(r"\s*([A-Za-z_]\w*)", item)
        if mm:
            out.append(mm.group(1))
    return out


def case_body(text: str, label: str) -> str:
    """Тело `case label:` до следующего case/default. Пусто — если case нет."""
    m = re.search(r"case " + label + r":(.*?)(?:\n\s*(?:case |default:|\}))", text, re.S)
    return m.group(1) if m else ""


def main() -> int:
    h = read("Inc/modbus_calib.h")
    c = read("Src/modbus_calib.c")
    r = read("Src/modbus_registers.c")
    m = read("Src/main.c", "cp1251")
    t = read("tests/host/calib_test.c")
    doc = read("README.md")
    hs, cs, rs = strip(h), strip(c), strip(r)
    D = defines(hs)

    sys.path.insert(0, str(ROOT / "tools"))
    import modbus_calib as py

    errors = []
    n = 0

    def check(cond, msg):
        nonlocal n
        n += 1
        if not cond:
            errors.append(msg)

    for req in ("MB_H_SNAP", "MB_H_CAL_CMD", "MB_H_CAL_PAGE", "MB_H_COUNT",
                "MB_CAL_FIRST", "MB_CAL_WINDOW", "MB_CAL_WINDOW_FLOATS",
                "MB_CAL_TOTAL", "MB_CAL_T_ANGTAB", "MB_CAL_T_BUFK",
                "MB_CAL_T_COUNT", "MB_E_WRITTEN", "MB_E_ANGTAB", "MB_E_BUFK",
                "MB_E_OFFSET", "MB_CAL_STOP", "MB_CAL_ANGTAB_RIGHT",
                "MB_CAL_ANGTAB_LEFT", "MB_CAL_OFFSET", "MB_CAL_BUFK"):
        check(req in D, f"в Inc/modbus_calib.h нет #{req}")

    # --- запись, FC06 ---
    check((D["MB_H_SNAP"], D["MB_H_CAL_CMD"], D["MB_H_CAL_PAGE"]) == (0, 1, 2),
          "адреса holding-регистров на проводе должны быть 0, 1, 2")
    check(D["MB_H_COUNT"] == 3, "REG_HOLDING_COUNT должен быть 3: SNAP, CAL_CMD, CAL_PAGE")
    check("MB_H_SNAP" in rs and "MB_H_COUNT" in rs,
          "modbus_registers.c должен отсекать запись вне MB_H_SNAP…MB_H_COUNT")
    check("MB_H_CAL_CMD" in cs and "MB_H_CAL_PAGE" in cs,
          "modbus_calib.c должен разбирать CAL_CMD и CAL_PAGE по константам из заголовка")
    check("count != 1" in rs, "FC06 должен принимать ровно один регистр (FC16 выключен)")
    check("mb_cal_write" in rs and "snapshot_latch" in rs,
          "modbus_registers.c должен передавать запись в mb_cal_write/snapshot_latch")

    # --- коды команд: запуск и завершение ---
    codes = {name: value for name, value in D.items() if re.match(r"MB_CAL_(STOP|ANG|OFF|BUF)", name)}
    check(sorted(codes.values()) == [0x10, 0x11, 0x12, 0x13, 0x14],
          f"коды команд должны идти подряд от 0x10: {sorted(codes.values())}")
    check(sorted(py.CMD) == sorted(codes.values()),
          f"коды команд разошлись: инструмент {sorted(py.CMD)}, прошивка {sorted(codes.values())}")
    for k, v in py.CMD.items():
        name = codes.get(f"MB_CAL_{v[0]}")
        check(name == k, f"код 0x{k:02X}: в заголовке MB_CAL_{v[0]} = {name}")
        body = case_body(cs, f"MB_CAL_{v[0]}")
        check(bool(body), f"в modbus_calib.c нет case MB_CAL_{v[0]}")
        st = re.search(r"encoder_state\s*=\s*(0x[0-9A-Fa-f]+)", body)
        check(st is not None and int(st.group(1), 16) == v[2],
              f"после 0x{k:02X} прошивка не ставит STATE=0x{v[2]:02X}")
        if k in py.DONE:                              # конечное состояние — в main.c
            state = py.DONE[k][0]
            check(re.search(r"encoder_state\s*=\s*0x%02X\s*;" % state, strip(m)) is not None,
                  f"main.c не переходит в STATE=0x{state:02X} (итог команды 0x{k:02X})")
    check("default:\n        return -2" in c or "return -2" in cs,
          "неизвестный код команды должен возвращать -2 (исключение 0x04)")

    # --- живой статус: перечень, порядок, размер ---
    regs = enum_names(hs)
    check(regs[:1] == ["MB_R_STATE"], f"первым в перечне должен идти MB_R_STATE, а не {regs[:1]}")
    check(regs == ["MB_R_" + x for x, _, _ in py.REGS],
          "REGS в инструменте разошёлся с перечнем mb_cal_reg_e в заголовке")
    check(len(regs) == 20, f"регистров статуса {len(regs)}, ожидалось 20")
    check((py.FIRST, py.WINDOW, py.WINDOW_FLOATS, py.TOTAL) ==
          (D["MB_CAL_FIRST"], D["MB_CAL_WINDOW"], D["MB_CAL_WINDOW_FLOATS"],
           D["MB_CAL_TOTAL"]),
          "адреса статуса и окна в инструменте не совпадают с заголовком")
    check(D["MB_CAL_FIRST"] == 11, "регистры 0…10 отданы снимку, калибровка начинается с 11")
    check(D["MB_CAL_WINDOW"] == D["MB_CAL_FIRST"] + len(regs),
          "окно выгрузки должно начинаться сразу за блоком статуса")
    check(D["MB_CAL_TOTAL"] == D["MB_CAL_WINDOW"] + 2 * D["MB_CAL_WINDOW_FLOATS"],
          "MB_CAL_TOTAL не равен окну плюс блок статуса")
    check(f"case {regs[-1]}" in cs, f"modbus_calib.c не отдаёт последний регистр {regs[-1]}")
    for name in regs:
        check(f"case {name}" in cs, f"modbus_calib.c не отдаёт {name}")
    check("MB_CAL_TOTAL" in rs and "MB_ENOREG" in rs,
          "modbus_registers.c должен ограничивать FC04 константой MB_CAL_TOTAL")

    # --- биты EEPROM ---
    check(sorted(D[f"MB_E_{x}"] for x in ("WRITTEN", "ANGTAB", "BUFK", "OFFSET")) == [0, 1, 2, 3],
          "биты EEPROM в заголовке должны быть 0…3")
    used = sorted(D[x] for x in re.findall(r"<<\s*(MB_E_\w+)", cs))
    check(used == [0, 1, 2, 3], f"в modbus_calib.c собрана другая маска: {used}")
    check(sorted(b for b, _ in py.EEPROM_BITS) == [0, 1, 2, 3],
          "расшифровка битов EEPROM в инструменте разошлась с прошивкой")

    # --- таблицы для выгрузки ---
    check(D["MB_CAL_T_ANGTAB"] == py.TABLES["ang_tab"][0] and
          D["MB_CAL_T_BUFK"] == py.TABLES["buf_k"][0],
          "идентификаторы таблиц разошлись между прошивкой и инструментом")
    check(D["MB_CAL_T_COUNT"] == len(py.TABLES),
          "число таблиц в заголовке и в инструменте разное")
    sizes = dict(re.findall(r"#define\s+(BIT_TAB_SIZE|ENCODER_PIXELS)\s+(\d+)",
                            read("Inc/encoder_vars.h")))
    check(int(sizes["BIT_TAB_SIZE"]) == py.TABLES["ang_tab"][1],
          "ang_tab: размер разошёлся с BIT_TAB_SIZE")
    check(int(sizes["ENCODER_PIXELS"]) == py.TABLES["buf_k"][1],
          "buf_k: размер разошёлся с ENCODER_PIXELS")
    check("MB_CAL_T_ANGTAB" in cs and "MB_CAL_T_BUFK" in cs,
          "modbus_calib.c должен выбирать таблицу по константам из заголовка")
    check("MB_CAL_WINDOW_FLOATS" in cs,
          "размер страницы должен браться из заголовка, а не из литерала")

    # --- порядок слов в окне: старший первым ---
    check("(uint16_t)(bits >> 16)" in cs,
          "в окне старшее слово float должно идти первым (чётный индекс)")
    check(re.search(r"\*buf\+\+ = \(UCHAR\)\(value >> 8\);\s*\n\s*\*buf\+\+ = \(UCHAR\)value;",
                    rs) is not None,
          "modbus_registers.c должен выкладывать регистры старшим байтом вперёд")

    # --- тест на хосте должен мерить те же границы ---
    check("UCHAR raw[2 * MB_CAL_TOTAL]" in t and "regs[MB_CAL_TOTAL]" in t,
          "calib_test.c должен читать FC04 до MB_CAL_TOTAL включительно")
    for name in regs:
        check(name in t or name.endswith("_LO"), f"calib_test.c не проверяет {name}")
    for code in sorted(py.CMD):
        label = f"MB_CAL_{py.CMD[code][0]}"
        check(label in t or f"0x{code:02X}" in t, f"calib_test.c не проверяет команду 0x{code:02X}")

    # --- порядок запуска и скорости: инструмент и документы ---
    cal_doc = read("docs/calibration_modbus.md")
    check(list(py.RUN_ORDER) == [0x14, 0x13, 0x11, 0x12],
          f"порядок запуска в инструменте странный: {py.RUN_ORDER}")
    check("`0x14` → `0x13` → `0x11` → `0x12` → `0x10`" in cal_doc,
          "в процедуре порядок команд описан иначе, чем в инструменте")
    check(set(py.RUN_INFO) == set(py.CMD) - {0x10},
          "для каждой пусковой команды (кроме стоп 0x10) должно быть описание "
          "скорости и ожидания")
    for code, (speed, revs, timeout) in sorted(py.RUN_INFO.items()):
        check(speed in cal_doc, f"скорость {speed} об/мин для 0x{code:02X} не в процедуре")
        check(speed in doc, f"скорость {speed} об/мин для 0x{code:02X} не в README")
        check(revs.split("…")[0] in cal_doc, f"ориентир {revs} оборотов для 0x{code:02X} не в процедуре")
        check(60.0 <= timeout <= 1800.0, f"ожидание 0x{code:02X} = {timeout} с вне здравого смысла")
    check(re.search(r"втор\w*\s+круг", " ".join(cal_doc.split())) and
          re.search(r"Повторить\s+`0x14`,\s+затем `0x13`", " ".join(cal_doc.split())),
          "в процедуре должен быть второй круг: повтор 0x14 и 0x13 после ang_tab")

    # --- адрес головки (slave id): выбор, границы, поиск на линии ---
    tool = read("tools/modbus_calib.py")
    check('"-s", "--slave"' in tool,
          "в инструменте должен быть явный выбор адреса ключом -s/--slave")
    check(re.search(r"def check_slave\(slave: int\)[^\n]*\n(?:.*\n)*?\s+if not 1 "
                    r"<= slave <= 247", tool) is not None,
          "адрес должен проверяться одной функцией check_slave()")
    check(tool.count("check_slave(a.slave)") >= 2,
          "проверка адреса должна быть и перед обменом по шине, и в режиме frames")
    check(hasattr(py, "mode_scan") and py.HELP.get("scan"),
          "режим scan (перебор адресов на линии) должен быть в инструменте")
    check("--from" in tool and "--to" in tool and "probe" in tool,
          "scan должен перебирать диапазон адресов одним запросом на адрес")
    check("scan" in doc or "scan" in cal_doc,
          "в документах должно быть написано, как узнать адрес головки")

    # --- разбор статуса: числа должны соответствовать прошивочным ---
    regs = [0x50, 37, 144, 2, 65, 64, 17536, 100, 0xFFF4,
            0xFFFF, 0xFFFD, 4, 11, 2, 5, 0x056F, 0x1234, 0x0001, 0x0000, 0b1111]
    d = py.decode(regs)
    check(d["STATE"] == 0x50 and d["RDY1"] == 37 and d["RDY2"] == 144,
          "одиночные слова разбираются неверно")
    check(d["PIX_MIN_MAX"] == -12, "PIX_MIN_MAX должен быть знаковым")
    check(abs(d["OFFSET_X256"] / 256 - 68.5) < 1e-9, "offset×256 должен делиться на 256")
    check(abs(d["ANGLE"] - 0x056F1234 / 1e6) < 1e-9, "угол собирается из двух слов старшим вперёд")
    check(abs(d["MINMAX"] + 3 * 3.6e-3) < 1e-12,
          "min_max: знаковые микроградусы должны печататься в угловых секундах")
    check(d["FRAMES"] == 0x00010000 and d["EEPROM"] == 0b1111,
          "счётчик кадров и биты EEPROM разобраны неверно")
    import contextlib
    import io
    buf = io.StringIO()
    with contextlib.redirect_stdout(buf):
        py.show(d)
    check("STATE=0x50" in buf.getvalue() and "min_max = -0.011″" in buf.getvalue(),
          "показ состояния должен печататься на разобранных ключах (после смены "
          "имён ключей в decode show обязан обновиться)")

    # --- режим калибровки ESP32 (esp32_master) — те же коды, порядок и ожидания, что в утилите ---
    esp_src = read("esp32_master/main/calib_mode.c")
    esp = strip(esp_src)
    ED = defines(esp)
    cmake = read("esp32_master/main/CMakeLists.txt")
    kconf = read("esp32_master/main/Kconfig.projbuild")
    poll_src = strip(read("esp32_master/main/poll_mode.c"))

    check(ED.get("H_CAL_CMD") == D["MB_H_CAL_CMD"] and ED.get("H_CAL_PAGE") == D["MB_H_CAL_PAGE"],
          "esp32 calib_mode.c: адреса CAL_CMD/CAL_PAGE не совпали с Inc/modbus_calib.h")
    check((ED.get("CAL_FIRST"), ED.get("CAL_WINDOW"), ED.get("CAL_WINDOW_FLOATS")) ==
          (D["MB_CAL_FIRST"], D["MB_CAL_WINDOW"], D["MB_CAL_WINDOW_FLOATS"]),
          "esp32 calib_mode.c: блок статуса, окно или размер страницы не совпали с заголовком")
    check(ED.get("CAL_STATUS_WORDS") == len(regs),
          "esp32 calib_mode.c: число регистров статуса не совпало с заголовком")
    check(ED.get("TABLE_ANGTAB") == py.TABLES["ang_tab"][0] and
          ED.get("ANGTAB_LEN") == py.TABLES["ang_tab"][1],
          "esp32 calib_mode.c: идентификатор или длина ang_tab не совпали с инструментом")
    check(ED.get("CMD_STOP") == 0x10, "esp32: CMD_STOP должен быть 0x10")
    for code, (name, _, _) in py.CMD.items():
        check(ED.get(f"CMD_{name}") == code,
              f"esp32 calib_mode.c: CMD_{name} != 0x{code:02X}")

    # таблица команд: STATE после записи, готово, скорости, ориентиры, таймауты
    rows = re.findall(r'\{CMD_(\w+),\s*"(\w+)",\s*"[^"]*",\s*(0x[0-9A-Fa-f]+),\s*(0x[0-9A-Fa-f]+),'
                      r'\s*"[^"]*",\s*"([^"]+)",\s*"([^"]+)",\s*(\d+)\}', esp_src)
    check(len(rows) == len(py.RUN_INFO),
          f"в calib_mode.c таблица CMDS: {len(rows)} строк, ожидалось {len(py.RUN_INFO)}")
    for macro, name, after, done, speed, revs, timeout in rows:
        code = ED.get(f"CMD_{macro}")
        check(code is not None and code in py.RUN_INFO and name == macro,
              f"строка CMDS для {macro} не соответствует команде инструмента")
        if code is None or code not in py.RUN_INFO:
            continue
        check(int(after, 16) == py.CMD[code][2],
              f"0x{code:02X}: STATE сразу после записи в calib_mode.c не совпадает с инструментом")
        check(int(done, 16) == py.DONE[code][0],
              f"0x{code:02X}: STATE «готово» в calib_mode.c не совпадает с инструментом")
        speed_py, revs_py, timeout_py = py.RUN_INFO[code]
        check((speed, revs, int(timeout)) == (speed_py, revs_py, int(timeout_py)),
              f"0x{code:02X}: скорость/ориентир/таймаут в calib_mode.c ({speed}, {revs}, {timeout}) "
              f"не совпадают с инструментом ({speed_py}, {revs_py}, {timeout_py})")

    # порядок запуска и второй круг
    order = re.search(r"RUN_ORDER\[\]\s*=\s*\{([^}]*)\}", esp)
    check(order is not None and [ED.get(x.strip()) for x in order.group(1).split(",")] == list(py.RUN_ORDER),
          "порядок RUN_ORDER в calib_mode.c не совпадает с инструментом")
    second = re.search(r"SECOND\[\]\s*=\s*\{([^}]*)\}", esp)
    check(second is not None and [ED.get(x.strip()) for x in second.group(1).split(",")] == [0x14, 0x13],
          "второй круг в calib_mode.c должен повторять 0x14, затем 0x13")
    check(re.search(r"ANGTAB_RIGHT.*?ANGTAB_LEFT", esp, re.S) is not None,
          "проходы ang_tab в calib_mode.c должны идти подряд: 0x11, затем 0x12")

    # Kconfig: значения по умолчанию из утилиты; сборка выбирает один режим
    check("default 500" in kconf and "ENCODER_CALIB_POLL_MS" in kconf,
          "период опроса по умолчанию (500 мс = --interval 0.5) должен быть в Kconfig")
    check("default 1" in kconf and "ENCODER_CALIB_SLAVE" in kconf,
          "адрес головки по умолчанию (1, как -s 1 в утилите) должен быть в Kconfig")
    check("CONFIG_ENCODER_MODE_CALIB" in cmake and "calib_mode.c" in cmake and "poll_mode.c" in cmake,
          "CMake должен подключать только один режим: calib_mode.c или poll_mode.c")
    check("H_CAL_CMD" not in poll_src and "CMD_BUFK" not in poll_src and "CAL_PAGE" not in poll_src,
          "рабочий режим (poll_mode.c) не должен знать о командах калибровки")

    # --- README ---
    for code in sorted(py.CMD):
        check(f"0x{code:02X}" in doc, f"в README.md нет упоминания команды 0x{code:02X}")
    check(str(D["MB_CAL_TOTAL"]) in doc, f"в README.md не указан размер FC04 ({D['MB_CAL_TOTAL']})")
    for name in ("calibration_modbus.md", "modbus_rtu.md"):
        check(name in doc, f"в README.md нет ссылки на {name}")
    check("0x10" in doc and "11…30" in doc and "31…62" in doc,
          "в README.md должны быть команды 0x10…0x14 и блоки регистров 11…30, 31…62")

    if errors:
        print("карта канала калибровки разошлась с определением:")
        for e in errors:
            print("  " + e)
        return 1
    print(f"карта калибровки совпадает в прошивке, тесте, инструменте и README — "
          f"проверено проверок: {n}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
