#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""Сверка extern-моста Inc/encoder_vars.h с определениями в Src/main.c.

Src/main.c этот заголовок не включает (исторический файл не правим), поэтому
несовпадение типа или размера массива компилятор не заметит: объявления живут в
разных translation unit'ах, а последствия — неверное чтение/запись памяти под
нос прошивки. Тест сравнивает имена, базовые типы и размеры массивов.

main.c сохранён в CP1251 (.editorconfig) — читаем именно так.

Запуск: python3 tests/encoder_vars_test.py
"""
from __future__ import annotations

import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
HEADER = ROOT / "Inc" / "encoder_vars.h"
MAIN = ROOT / "Src" / "main.c"

ALIASES = {}          # synonyms типов, если понадобятся: 'int' -> 'signed int'
CONSTS = {"BIT_TAB_SIZE": 144, "ENCODER_PIXELS": 128}


def norm_type(t: str) -> str:
    t = re.sub(r"__attribute__\s*\(\(.*?\)\)", " ", t, flags=re.S)
    t = t.replace("extern", " ")
    t = " ".join(t.split())
    return " ".join(ALIASES.get(w, w) for w in t.split())


def norm_size(s: str | None) -> int | None:
    if not s:
        return None
    s = s.strip().strip("[]").replace(" ", "")
    if s in CONSTS:
        return CONSTS[s]
    try:
        return int(s)
    except ValueError:
        return -1  # нестандартное выражение — пусть совпадёт только текстом


def main() -> int:
    if not HEADER.exists() or not MAIN.exists():
        raise SystemExit("нужны Inc/encoder_vars.h и Src/main.c")
    header = HEADER.read_text(encoding="utf-8")
    header = re.sub(r"/\*.*?\*/", " ", header, flags=re.S)
    # Комментарии и __attribute__ мешают сопоставлению типов — убираем сразу.
    mainc = MAIN.read_text(encoding="cp1251")
    mainc = re.sub(r"/\*.*?\*/", " ", mainc, flags=re.S)
    mainc = re.sub(r"//[^\n]*", " ", mainc)
    # __attribute__((...)) с вложенными скобками — регулярка ниже знает один
    # уровень вложенности, чего хватает для всех определений в main.c.
    mainc = re.sub(r"__attribute__\s*\(\((?:[^()]|\([^()]*\))*\)\)[ \t]?",
                    "", mainc, flags=re.S)

    # Константы самого заголовка должны соответствовать main.c.
    for name, want in CONSTS.items():
        m = re.search(r"#define[ \t]+" + name + r"[ \t]+(\d+)", header)
        if not m:
            raise SystemExit(f"в encoder_vars.h нет #define {name}")
        if int(m.group(1)) != want:
            raise SystemExit(f"{name}: в заголовке {m.group(1)}, ожидалось {want}")
    m = re.search(r"#define[ \t]+BIT_TAB_SIZE[ \t]+(\d+)", mainc)
    if not m or int(m.group(1)) != CONSTS["BIT_TAB_SIZE"]:
        raise SystemExit("BIT_TAB_SIZE в main.c отличается от encoder_vars.h")

    decls = re.findall(r"extern[ \t]+([\w \t\*]+?)[ \t]+(\w+)[ \t]*(?:\[([^\]]*)\])?[ \t]*;",
                       header)
    if not decls:
        raise SystemExit("в encoder_vars.h не найдено ни одного extern-объявления")

    errors: list[str] = []
    checked = 0
    for ctype, name, csize in decls:
        # Определение — строка без отступа (уровень файла): тип, имя,
        # необязательный [размер], инициализатор; заканчивается ';' или '{'
        # (многострочные инициализаторы вида `float buf_k[128] = {`).
        pat = (r"(?m)^([A-Za-z_][\w \t\*]*?)[ \t]+" + name +
               r"[ \t]*(\[[^\]]*\])?[ \t]*(?:=[^;\n]*)?[;{]")
        m = re.search(pat, mainc)
        if not m:
            errors.append(f"{name}: определение не найдено в Src/main.c")
            continue
        dtype, dsize = m.group(1).strip(), m.group(2)
        if "static" in dtype.split():
            errors.append(f"{name}: в main.c определение static — extern-мост не работает")
            continue
        if norm_type(dtype) != norm_type(ctype):
            errors.append(f"{name}: тип в main.c {norm_type(dtype)!r}, "
                          f"в заголовке {norm_type(ctype)!r}")
            continue
        hs, ms = norm_size(csize), norm_size(dsize)
        if hs is not None and ms is not None and hs != ms:
            errors.append(f"{name}: размер массива: main.c {ms}, заголовок {hs}")
            continue
        if (hs is None) != (ms is None):
            errors.append(f"{name}: в одном месте массив, в другом — скаляр "
                          f"(main.c {dsize!r}, заголовок {csize!r})")
            continue
        checked += 1
        print(f"  ok  {norm_type(ctype):<22} {name}"
              + (f"[{csize}]" if csize else ""))

    # Обратная проверка: все объявления заголовка уникальны и все имена из
    # карты modbus_calib.c действительно объявлены здесь.
    names = [n for _, n, _ in decls]
    dupes = sorted({n for n in names if names.count(n) > 1})
    if dupes:
        errors.append(f"дублируются в заголовке: {dupes}")
    needed = set(re.findall(r"\b(start_calibrate|auto_cal|pix_rdy_num1|pix_rdy_num2|"
                            r"ang_tab|pix_dif_tab1|pix_dif_tab2|pix_dif_num_avg|offset|"
                            r"offset_avg_num|start_offset_cal|offset_phase|offset_cur|"
                            r"offset_minmax|offset_found|offset_start|offset_end|"
                            r"start_angk_cal|anglek_phase|anglek_cur|buf_k|buf_x3|"
                            r"pix_min_max|min_max|avg_minmax_num|rev_left_cnt|"
                            r"rev_right_cnt|rev_en|serrcnt1|serrcnt2|sector|cur_ang_E|"
                            r"errorflag|cycles_max|Temperature|backlight_width_ticks|"
                            r"backlight_width_en|ang_tab_rdy|buf_k_rdy|offset_rdy|"
                            r"lasdac_rdy|i2c3_tx_wp|encoder_state)\b",
                            (ROOT / "Src" / "modbus_calib.c").read_text(encoding="utf-8")))
    missing = sorted(needed - set(names))
    if missing:
        errors.append(f"modbus_calib.c использует необъявленные: {missing}")

    if errors:
        print("\nРАСХОЖДЕНИЕ encoder_vars.h ↔ main.c:")
        for e in errors:
            print("  !", e)
        return 1
    print(f"\nИнклюд encoder_vars.h согласован с main.c: "
          f"проверено объявлений — {checked}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
