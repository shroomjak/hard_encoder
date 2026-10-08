#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Сличение карты регистров: Inc/modbus_calib.h ↔ tools/modbus_calib.py.

Карта принадлежит прошивке (Inc/modbus_calib.h), а утилита и документация держат
свои копии. Тест ловит рассинхрон имён, порядка, адресов и номеров битов — то
есть ситуацию, когда оператор по инструкции читает не тот регистр.

Запуск: python3 tests/calib_map_test.py
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "tools"))

import modbus_calib as tool  # noqa: E402

HEADER = ROOT / "Inc" / "modbus_calib.h"


def strip_comments(text: str) -> str:
    text = re.sub(r"/\*.*?\*/", " ", text, flags=re.S)
    return re.sub(r"//[^\n]*", " ", text)


class Header:
    """Константы заголовка: #define (не функции-макросы) и члены enum'ов."""

    def __init__(self, text: str):
        self.text = text
        self.defines: dict[str, str] = {}
        for m in re.finditer(r"#define[ \t]+([A-Za-z_]\w*)(\([^)]*\))?[ \t]*([^\n]*)", text):
            name, params, expr = m.group(1), m.group(2), m.group(3).strip()
            if params:                       # функции-макросы не считаем
                continue
            if not re.fullmatch(r"[\w\s+\-*/().]+", expr):
                continue                     # строки, void* и прочее — не константы
            self.defines[name] = expr
        self.enums: dict[str, int] = {}
        for block in re.findall(r"enum\s*\{([^}]*)\}", text, flags=re.S):
            value = -1
            for item in block.split(","):
                item = item.strip()
                if not item:
                    continue
                if "=" in item:
                    name, expr = item.split("=", 1)
                    value = self.value(expr)
                else:
                    name, value = item, value + 1
                self.enums[name.strip()] = value

    def resolve(self, name: str) -> int:
        """Число по имени константы: сначала enum'ы, затем #define (рекурсивно)."""
        if name in self.enums:
            return self.enums[name]
        if name in self.defines:
            return self.value(self.defines[name])
        raise SystemExit(f"неизвестная константа {name}")

    def value(self, expr: str) -> int:
        """Вычисляет константное выражение заголовка (числа, скобки, имена)."""
        expr = re.sub(r"\b(0[xX][0-9a-fA-F]+|\d+)[uUlL]+\b", r"\1", expr.strip())
        names = [n for n in re.findall(r"(?<![\w.])[A-Za-z_]\w*", expr)
                 if n not in ("int", "u", "l")]
        py = expr
        for n in dict.fromkeys(names):
            py = re.sub(r"(?<![\w.])" + n + r"\b", str(self.resolve(n)), py)
        if not re.fullmatch(r"[0-9a-fA-FxXeE+\-*/() _]+", py):
            raise SystemExit(f"не вычисляемое выражение: {expr!r} -> {py!r}")
        return int(eval(py, {"__builtins__": {}}, {}))

    def define(self, name: str) -> int:
        if name not in self.defines:
            raise SystemExit(f"в заголовке нет #define {name}")
        return self.value(self.defines[name])

    def defines_with_prefix(self, prefix: str) -> dict[str, int]:
        return {n: self.value(e) for n, e in self.defines.items()
                if n.startswith(prefix)}

    def enum_names(self, start: str, stop: str) -> list[str]:
        """Имена членов enum'а от start до stop (не включая stop)."""
        i, j = self.text.index(start), self.text.index(stop, self.text.index(start))
        body = self.text[i:j]
        names = []
        for m in re.finditer(r"\b([A-Za-z_]\w*)\b\s*(?:=[^,\n]*)?,?", body):
            n = m.group(1)
            if n in ("enum", "MB_CAL_INPUT_FIRST"):
                continue
            if n not in names:
                names.append(n)
        return names


def main() -> int:

    if not HEADER.exists():
        raise SystemExit(f"нет файла {HEADER}")
    text = strip_comments(HEADER.read_text(encoding="utf-8"))
    H = Header(text)
    errors: list[str] = []

    # --- holding-регистры --------------------------------------------------
    holding = H.defines_with_prefix("MB_H_REG_")
    for name, addr in tool.HOLDING.items():
        key = "MB_H_REG_" + name
        if holding.get(key) != addr:
            errors.append(f"holding {name}: прошивка {holding.get(key)}, утилита {addr}")
    if holding.get("MB_H_REG_COUNT") != tool.HOLDING_COUNT:
        errors.append(f"MB_H_REG_COUNT: прошивка {holding.get('MB_H_REG_COUNT')}, "
                      f"утилита {tool.HOLDING_COUNT}")

    # --- ключ, режим, коды команд ------------------------------------------
    if H.define("MB_CAL_KEY_UNLOCK") != tool.KEY_UNLOCK:
        errors.append("расхождение по MB_CAL_KEY_UNLOCK")
    for prefix, value in (("MB_CAL_CFG_RELAX", tool.CFG_RELAX),
                          ("MB_CAL_CFG_KEEP", tool.CFG_KEEP)):
        if H.define(prefix) != value:
            errors.append(f"расхождение по {prefix}")
    cmd_codes = {v for k, v in H.defines_with_prefix("MB_CAL_CMD_").items()
                 if re.fullmatch(r"MB_CAL_CMD_(STOP|ANGTAB_RIGHT|ANGTAB_LEFT|OFFSET|BUFK)", k)}
    if cmd_codes != set(tool.COMMANDS):
        errors.append(f"набор кодов команд: прошивка {sorted(cmd_codes)}, "
                      f"утилита {sorted(tool.COMMANDS)}")

    # --- живой блок: имена и порядок ---------------------------------------
    names = [n[len("CAL_R_"):] for n in
             H.enum_names("CAL_R_BLOCK_ID", "CAL_R_INPUT_END")]
    tool_names = [n for n, _, _ in tool.LIVE_REGS]
    if names != tool_names:
        for i, (a, b) in enumerate(zip(names, tool_names)):
            if a != b:
                errors.append(f"порядок CAL_R_* разошёлся с первого расхождения "
                              f"(индекс {i}): прошивка {a}, утилита {b}")
                break
        else:
            errors.append(f"число живых регистров: прошивка {len(names)}, "
                          f"утилита {len(tool_names)}")

    if H.define("MB_CAL_INPUT_FIRST") != tool.INPUT_FIRST:
        errors.append("расхождение по MB_CAL_INPUT_FIRST")
    live_count = len(names)
    if H.define("MB_CAL_INPUT_COUNT") != live_count:
        errors.append("MB_CAL_INPUT_COUNT не равен числу элементов enum")
    if tool.WINDOW_FIRST != tool.INPUT_FIRST + live_count:
        errors.append("утилита: окно стоит не сразу за живым блоком")
    for name, want in (("MB_CAL_WINDOW_FLOATS", tool.WINDOW_FLOATS),
                       ("MB_CAL_WINDOW_FIRST", tool.WINDOW_FIRST),
                       ("MB_CAL_WINDOW_COUNT", tool.WINDOW_COUNT),
                       ("MB_CAL_WINDOW_LAST", tool.WINDOW_LAST),
                       ("MB_CAL_INPUT_TOTAL", tool.INPUT_TOTAL)):
        got = H.define(name)
        if got != want:
            errors.append(f"{name}: прошивка {got}, утилита {want}")
    if H.define("MB_CAL_BLOCK_ID_VALUE") != 0xCA10:
        errors.append("расхождение по маркеру блока")

    # --- биты флагов и предупреждений ---------------------------------------
    for prefix, table, label in (("CAL_F_", tool.FLAGS_BITS, "FLAGS"),
                                 ("CAL_D_", tool.DIAG_BITS, "DIAG")):
        bits = H.defines_with_prefix(prefix)
        want = {prefix + n: b for b, n, _ in table}
        if bits != want:
            bad = [k for k in set(bits) | set(want) if bits.get(k) != want.get(k)]
            errors.append(f"биты {label} расходятся: {sorted(bad)}")

    # --- таблицы и состояния -------------------------------------------------
    # Имена идентификаторов таблиц в C — отдельная сверка: в CLI утилиты имена
    # читаемые («pix_dif_tab1»), в прошивке — компактные (CAL_T_PIXDIF1).
    table_macro = {"ang_tab": "CAL_T_ANG_TAB", "buf_k": "CAL_T_BUF_K",
                   "pix_dif_tab1": "CAL_T_PIXDIF1", "pix_dif_tab2": "CAL_T_PIXDIF2",
                   "offset_minmax": "CAL_T_OFFSET_MINMAX"}
    tables = H.defines_with_prefix("CAL_T_")
    want_tables = {macro: tool.TABLES[name][0] for name, macro in table_macro.items()}
    want_tables["CAL_T_COUNT"] = len(tool.TABLES)
    if tables != want_tables:
        errors.append(f"идентификаторы таблиц: прошивка {tables}, утилита {want_tables}")

    # Длины таблиц в утилите обязаны совпадать с константами encoder_vars.h
    # (иначе dump читает не то количество страниц).
    ev = Header(strip_comments((ROOT / "Inc" / "encoder_vars.h").read_text(encoding="utf-8")))
    sizes = {"ang_tab": ev.define("BIT_TAB_SIZE"), "buf_k": ev.define("ENCODER_PIXELS"),
             "pix_dif_tab1": ev.define("BIT_TAB_SIZE"),
             "pix_dif_tab2": ev.define("BIT_TAB_SIZE"),
             "offset_minmax": ev.define("ENCODER_PIXELS")}
    for name, (_, length, _) in tool.TABLES.items():
        pages = -(-length // tool.WINDOW_FLOATS)
        if sizes[name] != length:
            errors.append(f"длина таблицы {name}: утилита {length}, "
                          f"encoder_vars.h {sizes[name]}")
        if pages * tool.WINDOW_FLOATS < length:
            errors.append(f"{name}: страниц {pages} не хватает на {length} элементов")
    states = H.defines_with_prefix("MB_CAL_STATE_")
    for code, (name, desc, running) in tool.COMMANDS.items():
        want_state = states.get(f"MB_CAL_STATE_{name}")
        if want_state is not None and want_state != running:
            errors.append(f"STATE для {name}: прошивка 0x{want_state:02X}, "
                          f"утилита 0x{running:02X}")
    for code, targets, _ in tool.SEQUENCE:
        name = tool.COMMANDS[code][0]
        for t in targets:
            if not any(v == t for k, v in states.items() if "RDY" in k):
                errors.append(f"утилита ждёт 0x{t:02X} после 0x{name[2:]} — "
                              f"такого кода нет среди MB_CAL_STATE_*")

    if errors:
        print("КАРТА РАСХОДИТСЯ:")
        for e in errors:
            print("  !", e)
        return 1
    print(f"Карта совпадает: живых регистров {live_count} "
          f"(адреса {tool.INPUT_FIRST}…{tool.WINDOW_FIRST - 1}), "
          f"окно выгрузки {tool.WINDOW_COUNT} регистра "
          f"({tool.WINDOW_FIRST}…{tool.WINDOW_LAST}), "
          f"holding-регистров {tool.HOLDING_COUNT}, всего FC04 {tool.INPUT_TOTAL}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
