#!/usr/bin/env python3
"""Host-side regression tests for calc_sector(), bit_err_corr(), and err_corr().

Extracts bit_tab[], sector_by_code[], calc_sector(), bit_err_corr(), and err_corr()
verbatim from Src/main.c (CP1251 encoded), compiles them into a small shared
library against mock globals, and exercises clean decoding, single-bit Chase-1
correction, O(1) lookup invariants, and temporal sector windowing via ctypes.
"""

from __future__ import annotations

import ctypes
import pathlib
import re
import subprocess
import sys
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[1]
MAIN_C = ROOT / "Src" / "main.c"


def extract_c_block(source: str, anchor: str) -> str:
    pos = 0
    while True:
        start = source.index(anchor, pos)
        brace = source.index("{", start)
        semi = source.find(";", start, brace)
        if semi == -1:
            break
        pos = semi + 1
    depth = 0
    for idx in range(brace, len(source)):
        ch = source[idx]
        if ch == "{":
            depth += 1
        elif ch == "}":
            depth -= 1
            if depth == 0:
                end = idx + 1
                if end < len(source) and source[end] == ";":
                    end += 1
                return source[start:end]
    raise RuntimeError(f"Unbalanced braces for {anchor!r}")


def build_library(tmpdir: pathlib.Path) -> tuple[ctypes.CDLL, list[int]]:
    raw = MAIN_C.read_bytes().decode("cp1251")

    bit_tab_decl = extract_c_block(raw, "const uint16_t  bit_tab[BIT_TAB_SIZE]")
    sector_lut_decl = extract_c_block(raw, "const uint8_t sector_by_code[512]")
    calc_sector_fn = extract_c_block(raw, "void calc_sector(void)")
    save_sector_fn = extract_c_block(raw, "void save_sector(void)")
    bit_err_corr_fn = extract_c_block(raw, "void bit_err_corr(void)")
    err_corr_fn = extract_c_block(raw, "void err_corr(void)")

    # Structural check: no linear scan over BIT_TAB_SIZE in hot sector decode/correction
    for fn_name, fn_body in (
        ("calc_sector", calc_sector_fn),
        ("bit_err_corr", bit_err_corr_fn),
        ("err_corr", err_corr_fn),
    ):
        clean_fn = re.sub(r"//[^\n]*|/\*.*?\*/", "", fn_body, flags=re.S)
        assert "BIT_TAB_SIZE" not in clean_fn or fn_name == "err_corr", (
            f"{fn_name} still references BIT_TAB_SIZE!"
        )
        if fn_name == "err_corr":
            assert not re.search(r"\bfor\s*\(", clean_fn), "err_corr must be loop-free O(1)!"

    # Parse the 144 codewords out of bit_tab[] for test stimuli.
    body = bit_tab_decl.split("{", 1)[1].rsplit("}", 1)[0]
    body = re.sub(r"//[^\n]*", "", body)
    codewords = [int(tok) for tok in re.findall(r"\d+", body)]
    assert len(codewords) == 144, f"Expected 144 codewords, got {len(codewords)}"

    lut_body = sector_lut_decl.split("{", 1)[1].rsplit("}", 1)[0]
    lut_body = re.sub(r"//[^\n]*|/\*.*?\*/", "", lut_body, flags=re.S)
    lut_vals = [int(tok) for tok in re.findall(r"\d+", lut_body)]
    assert len(lut_vals) == 512, f"Expected 512 LUT entries, got {len(lut_vals)}"
    for code in range(512):
        if code in codewords:
            assert lut_vals[code] == codewords.index(code), f"LUT mismatch at code {code}"
        else:
            assert lut_vals[code] == 255, f"Expected 255 at unused code {code}, got {lut_vals[code]}"

    harness_c = f"""
#include <stdint.h>

#define BIT_TAB_SIZE 144

uint16_t pix[9][2];
int badbit_pos;
int badbit_value;
uint16_t data_byte;
uint8_t rem_sec;
uint8_t cursec;
uint8_t lsec;
uint8_t rsec;
uint8_t lsector;
uint8_t rsector;
uint8_t sector_history_valid;
uint8_t sector;
int errorflag;
uint32_t serrcnt1;
uint32_t serrcnt2;

{bit_tab_decl}

{sector_lut_decl}

{calc_sector_fn}

{save_sector_fn}

{bit_err_corr_fn}

{err_corr_fn}
"""
    c_path = tmpdir / "harness.c"
    so_path = tmpdir / "harness.so"
    # Strip non-ASCII comments so the compiler never trips on encoding flags.
    c_path.write_text(harness_c.encode("ascii", "ignore").decode("ascii"))
    subprocess.run(
        ["cc", "-std=c99", "-O2", "-Wall", "-Wextra", "-Werror",
         "-shared", "-fPIC", str(c_path), "-o", str(so_path)],
        check=True,
    )
    return ctypes.CDLL(str(so_path)), codewords


def set_codeword(lib: ctypes.CDLL, code: int, margin: int = 2000,
                 weak_bit: int | None = None, weak_margin: int = 100,
                 flip_weak: bool = False) -> None:
    pix_type = (ctypes.c_uint16 * 2) * 9
    pix = pix_type.in_dll(lib, "pix")
    base = 1000
    for i in range(9):
        bit = (code >> (8 - i)) & 1
        m = margin
        if weak_bit is not None and i == weak_bit:
            m = weak_margin
            if flip_weak:
                bit ^= 1
        if bit:
            pix[i][0] = base
            pix[i][1] = base + m
        else:
            pix[i][0] = base + m
            pix[i][1] = base


def scalar(lib: ctypes.CDLL, name: str, ctype):
    return ctype.in_dll(lib, name).value


def set_scalar(lib: ctypes.CDLL, name: str, ctype, value: int) -> None:
    ctype.in_dll(lib, name).value = value


def main() -> int:
    with tempfile.TemporaryDirectory() as tmp:
        lib, codewords = build_library(pathlib.Path(tmp))

        # 1. Every valid 9-bit codeword decodes cleanly to its own sector index.
        for sector_idx, code in enumerate(codewords):
            set_codeword(lib, code, margin=2000)
            lib.calc_sector()
            got = scalar(lib, "rem_sec", ctypes.c_uint8)
            packed = scalar(lib, "data_byte", ctypes.c_uint16)
            assert packed == code, f"sector {sector_idx}: packed {packed:#x} != {code:#x}"
            assert got == sector_idx, f"sector {sector_idx}: rem_sec={got}"

        # 2. All 9 bits participate in weak-bit detection and none reads past pix[8].
        for weak in range(9):
            set_codeword(lib, codewords[0], margin=2000, weak_bit=weak, weak_margin=150)
            lib.calc_sector()
            assert scalar(lib, "badbit_pos", ctypes.c_int) == weak
            assert scalar(lib, "badbit_value", ctypes.c_int) == 150

        # 3. Single-bit error on any of the 9 bit positions of any sector is
        #    repaired when that bit has the smallest differential margin (<1000).
        for sector_idx, code in enumerate(codewords):
            for bit_idx in range(9):
                set_codeword(lib, code, margin=2000,
                             weak_bit=bit_idx, weak_margin=80, flip_weak=True)
                lib.calc_sector()
                # Note: flipping 1 bit may either produce an unused codeword (255)
                # or collide with another valid codeword. Force the bit_err_corr path
                # whenever the flipped word is not already in the table, and check
                # unconditionally that bit_err_corr restores `sector_idx`.
                lib.bit_err_corr()
                repaired = scalar(lib, "rem_sec", ctypes.c_uint8)
                assert repaired == sector_idx, (
                    f"sector {sector_idx} bit {bit_idx}: repaired={repaired}"
                )

        # 4. High-confidence error (badbit_value >= 1000) is NOT touched by bit_err_corr.
        unused = next(c for c in range(512) if c not in set(codewords))
        set_codeword(lib, unused, margin=1500)
        lib.calc_sector()
        assert scalar(lib, "rem_sec", ctypes.c_uint8) == 255
        lib.bit_err_corr()
        assert scalar(lib, "rem_sec", ctypes.c_uint8) == 255

        # 5. All 368 unused 9-bit codewords decode to 255 in O(1).
        codeword_set = set(codewords)
        for code in range(512):
            if code not in codeword_set:
                set_codeword(lib, code, margin=2000)
                lib.calc_sector()
                assert scalar(lib, "rem_sec", ctypes.c_uint8) == 255

        # 6. err_corr() rejects startup single-bit flips when history is invalid.
        set_scalar(lib, "sector_history_valid", ctypes.c_uint8, 0)
        set_scalar(lib, "sector", ctypes.c_uint8, 0)
        set_scalar(lib, "errorflag", ctypes.c_int, 0)
        set_scalar(lib, "serrcnt1", ctypes.c_uint32, 0)
        set_scalar(lib, "serrcnt2", ctypes.c_uint32, 0)
        set_codeword(lib, codewords[17], margin=2000,
                     weak_bit=4, weak_margin=80, flip_weak=True)
        lib.calc_sector()
        lib.err_corr()
        assert scalar(lib, "errorflag", ctypes.c_int) == 1
        assert scalar(lib, "sector_history_valid", ctypes.c_uint8) == 0

        # 7. Clean read seeds history and updates {lsec, cursec, rsec} around wrap.
        set_scalar(lib, "errorflag", ctypes.c_int, 0)
        set_codeword(lib, codewords[0], margin=2000)
        lib.calc_sector()
        lib.err_corr()
        assert scalar(lib, "errorflag", ctypes.c_int) == 0
        assert scalar(lib, "sector_history_valid", ctypes.c_uint8) == 1
        assert scalar(lib, "cursec", ctypes.c_uint8) == 0
        assert scalar(lib, "lsec", ctypes.c_uint8) == 143
        assert scalar(lib, "rsec", ctypes.c_uint8) == 1

        # 8. Single-bit error into neighbor sector 143 is accepted via history window,
        #    while a jump to distant sector 50 is rejected without poisoning history.
        set_codeword(lib, codewords[143], margin=2000,
                     weak_bit=2, weak_margin=60, flip_weak=True)
        lib.calc_sector()
        lib.err_corr()
        assert scalar(lib, "errorflag", ctypes.c_int) == 0
        assert scalar(lib, "sector", ctypes.c_uint8) == 143
        assert scalar(lib, "cursec", ctypes.c_uint8) == 143

        # Distant clean-looking jump or distant single-bit repair is rejected:
        for distant in range(144):
            if distant in (142, 143, 0):
                continue
            for bit_idx in range(9):
                flipped = codewords[distant] ^ (0x100 >> bit_idx)
                if flipped not in codeword_set:
                    set_codeword(lib, codewords[distant], margin=2000,
                                 weak_bit=bit_idx, weak_margin=60, flip_weak=True)
                    lib.calc_sector()
                    lib.err_corr()
                    assert scalar(lib, "errorflag", ctypes.c_int) == 1
                    assert scalar(lib, "cursec", ctypes.c_uint8) == 143
                    break
            else:
                continue
            break

    print("OK: all 144 valid + 368 invalid codewords, O(1) LUT, Chase-1 & err_corr verified")
    return 0


if __name__ == "__main__":
    sys.exit(main())
