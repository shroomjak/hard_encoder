#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Регрессионный тест декодера сектора и побитовой коррекции.

Проверяются РЕАЛЬНЫЕ функции calc_sector() и bit_err_corr(): их исходный текст
и таблица bit_tab[] извлекаются прямо из Src/main.c, оборачиваются в заглушку
и компилируются хостовым gcc. Копии кода в тесте нет, поэтому тест не может
«разойтись» с прошивкой.

Что проверяется:
  1. Чтение без ошибок: все 144 кода декодируются в правильный номер сектора,
     badbit_pos всегда лежит в диапазоне 0..8 (контроль дефекта A - выхода
     за границу массива pix[9][2]).
  2. Одиночная битовая ошибка во всех 144*9 = 1296 комбинаций: ошибка должна
     детектироваться проверкой непрерывности сектора и исправляться
     bit_err_corr() (контроль дефекта B - маски разряда).
  3. Порог достоверности: при badbit_value >= 1000 коррекция не применяется.

Запуск:  python3 tests/sector_decode_test.py
Требуется: gcc. Исходник Src/main.c читается в кодировке CP1251 (см. .gitattributes).
"""

import io
import os
import re
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MAIN_C = os.path.join(ROOT, "Src", "main.c")


def grab_function(src, signature):
    """Вырезать тело функции по сигнатуре, считая фигурные скобки."""
    i = src.index(signature)
    k = src.index("{", i)
    depth = 0
    while True:
        if src[k] == "{":
            depth += 1
        elif src[k] == "}":
            depth -= 1
            if depth == 0:
                break
        k += 1
    return src[i:k + 1]


HARNESS = r'''
#include <stdio.h>
#include <stdint.h>

#define BIT_TAB_SIZE 144

%(bit_tab)s

int16_t  pix[9][2];
float    k_one = 1.0f, k_zero = 1.0f;   /* как в main.c: лежат сразу за pix[] */
int32_t  badbit_pos, badbit_value;
uint16_t data_byte;
uint8_t  rem_sec;

%(calc_sector)s

%(bit_err_corr)s

/* --- имитация find_datablock(): код -> "аналоговые" уровни pix[i][0..1] --- */
#define LO 100
#define HI 3700

static void make_pix(uint16_t code, int weak_bit, int weak_margin)
{
    int mid = (LO + HI) / 2;
    for (int i = 0; i < 9; i++) {
        int bit = (code >> (8 - i)) & 1;
        int y   = bit ? HI : LO;
        if (i == weak_bit)                    /* бит "подтянут" к порогу */
            y = bit ? (mid + weak_margin / 2) : (mid - weak_margin / 2);
        pix[i][0] = (int16_t)(HI - y);
        pix[i][1] = (int16_t)(y - LO);
    }
}

static int in_window(int rem, int j)
{
    int l = (j + BIT_TAB_SIZE - 1) %% BIT_TAB_SIZE;
    int r = (j + 1) %% BIT_TAB_SIZE;
    return (rem == j) || (rem == l) || (rem == r);
}

int main(void)
{
    int fail = 0;

    /* --- 1. чтение без ошибок --- */
    int clean_ok = 0, pos_ok = 1;
    for (int j = 0; j < BIT_TAB_SIZE; j++) {
        make_pix(bit_tab[j], -1, 0);
        calc_sector();
        if (rem_sec == j) clean_ok++;
        if (badbit_pos < 0 || badbit_pos > 8) pos_ok = 0;
    }
    printf("1) чтение без ошибок:            %%3d / %%3d секторов верно; "
           "badbit_pos в 0..8: %%s\n",
           clean_ok, BIT_TAB_SIZE, pos_ok ? "да" : "НЕТ (выход за массив pix[])");
    if (clean_ok != BIT_TAB_SIZE || !pos_ok) fail = 1;

    /* --- 2. одиночные битовые ошибки --- */
    int total = 0, detected = 0, corrected = 0, silent = 0, pos_bad = 0;
    for (int j = 0; j < BIT_TAB_SIZE; j++) {
        for (int b = 0; b < 9; b++) {
            total++;
            uint16_t corrupted = bit_tab[j] ^ (0x100 >> b);
            make_pix(corrupted, b, 400);      /* запас 400 < порога 1000 */
            calc_sector();
            if (badbit_pos != b) pos_bad++;
            if (in_window(rem_sec, j)) { silent++; continue; }
            detected++;
            bit_err_corr();
            if (rem_sec == j) corrected++;
        }
    }
    printf("2) одиночные битовые ошибки:     всего %%d, не детектируется %%d, "
           "неверно найден худший бит %%d\n", total, silent, pos_bad);
    printf("   исправлено bit_err_corr():    %%d / %%d  (%%.1f%%%%)\n",
           corrected, detected, detected ? 100.0 * corrected / detected : 0.0);
    if (silent != 0 || pos_bad != 0 || corrected != detected) fail = 1;

    /* --- 3. порог достоверности --- */
    make_pix(bit_tab[7] ^ 0x010, 4, 2000);    /* запас 2000 > порога 1000 */
    calc_sector();
    uint16_t before = data_byte;
    bit_err_corr();
    printf("3) badbit_value = %%d (>1000):   data_byte %%s\n",
           (int)badbit_value, before == data_byte ? "не изменён (верно)"
                                                  : "ИЗМЕНЁН (ошибка)");
    if (before != data_byte) fail = 1;

    printf("\n%%s\n", fail ? "ТЕСТ НЕ ПРОЙДЕН" : "ВСЕ ПРОВЕРКИ ПРОЙДЕНЫ");
    return fail;
}
'''


def main():
    src = io.open(MAIN_C, encoding="cp1251", newline="").read()

    bit_tab = re.search(
        r"(const uint16_t\s+bit_tab\[BIT_TAB_SIZE\]\s*=\s*\{.*?\};)", src, re.S
    ).group(1)

    code = HARNESS % {
        "bit_tab": bit_tab,
        "calc_sector": grab_function(src, "void calc_sector(void) {"),
        "bit_err_corr": grab_function(src, "void bit_err_corr(void) {"),
    }

    tmp = tempfile.mkdtemp(prefix="sector_test_")
    c_file = os.path.join(tmp, "sector_decode_test.c")
    exe = os.path.join(tmp, "sector_decode_test")
    io.open(c_file, "w", encoding="utf-8").write(code)

    cc = subprocess.run(
        ["gcc", "-std=c11", "-Wall", "-Wextra", "-O1", "-o", exe, c_file],
        capture_output=True, text=True,
    )
    if cc.stderr.strip():
        print("--- предупреждения компилятора ---")
        print(cc.stderr.strip())
        print("----------------------------------")
    if cc.returncode != 0:
        print("ОШИБКА КОМПИЛЯЦИИ")
        return 2
    # Выход за границу массива компилятор ловит сам - считаем это провалом теста.
    if "aggressive-loop-optimizations" in cc.stderr or "array-bounds" in cc.stderr:
        print("ОБНАРУЖЕН ВЫХОД ЗА ГРАНИЦУ МАССИВА (undefined behavior)")
        return 1

    result = subprocess.run(
        [exe],
        capture_output=True,
        encoding="utf-8",
    )

    if result.stdout:
        print(result.stdout, end="")

    if result.stderr:
        print(result.stderr, end="", file=sys.stderr)

    return result.returncode


if __name__ == "__main__":
    sys.exit(main())
