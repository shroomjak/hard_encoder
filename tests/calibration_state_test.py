#!/usr/bin/env python3
"""Команда Modbus -> диспетчер encoder_calibration_step() -> реальные алгоритмы
main.c (angk_cal, offset_cal, angtab_cal, mes_piks, BR4_C) -> LIVE-состояние.

Из CP1251 Src/main.c извлекаются реальные тела функций и объявления их переменных;
линкуются настоящие Src/encoder_calibration.c, Src/modbus_registers.c и
Src/snapshot_registers.c. Проверяются:
  - 4-этапная последовательность калибровки 0x14 -> 0x13 -> 0x11 -> 0x12 -> 0x10;
  - покрытие всего рабочего диапазона пикселей [6..126] в angk_cal() и точность
    субпиксельной оценки mes_piks()/BR4_C() на краевых реперах (startpixel=6 и 107);
  - сброс пикового детектора buf_x3 между фазами 1 и 2, защита от деления на ноль
    (отсутствие Inf/NaN) и ограничение числа итераций фазы 2;
  - расширение окна offset_cal() и защита от одновременного запуска автоматов.
Запуск: python3 tests/calibration_state_test.py
"""
from pathlib import Path
import os
import re
import shlex
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def function(source, signature):
    pos = 0
    while True:
        start = source.index(signature, pos)
        brace = source.index('{', start)
        semi = source.find(';', start, brace)
        if semi == -1:
            break
        pos = semi + 1
    depth = 0
    for end in range(brace, len(source)):
        depth += (source[end] == '{') - (source[end] == '}')
        if depth == 0:
            return source[start:end + 1]
    raise ValueError(signature)


def main():
    source = (ROOT / 'Src/main.c').read_text(encoding='cp1251')
    # Strip comments before extraction; braces in documentation are not C syntax.
    source = re.sub(r'/\*.*?\*/|//[^\n]*', '', source, flags=re.S)
    defines = '\n'.join(re.findall(
        r'^#define\s+(?:M_PI|BIT_TAB_SIZE|SECTOR_SIZE|STARTPIXEL_MIN|STARTPIXEL_MAX|'
        r'ANGK_\w+|EEPROM_DEVICE_PAGE\d|eeprom_\w+)\b[^\n]*',
        source, flags=re.M))
    variables = source[source.index('int pix_dif_num_avg='):source.index('unsigned char encoder_state=')]
    br4_pixels = source[source.index('float ampl;'):source.index('float r_angle;')]
    crc_table = re.search(r'static\s+unsigned int crc_16_table\[16\].*?;', source, re.S).group()
    functions = '\n'.join(function(source, sig) for sig in [
        'unsigned short crc_16_step(',
        'float BR4_C (',
        'float mes_piks(',
        'void angtab_cal(',
        'void offset_cal(',
        'void angk_cal(',
    ])
    harness = (ROOT / 'tests/host/calibration_state_harness.c').read_text()
    generated = harness.replace(
        '/* EXTRACTED_DECLARATIONS */',
        defines + '\n' + variables + '\n' + br4_pixels + '\n' + crc_table,
    )
    generated = generated.replace('/* EXTRACTED_FUNCTIONS */', functions)
    with tempfile.TemporaryDirectory() as directory:
        path = Path(directory)
        (path / 'state_test.c').write_text(generated)
        subprocess.run(['cc', '-std=c99', '-Wall', '-Wextra', '-Werror',
                        *shlex.split(os.environ.get('CFLAGS', '')),
                        '-Itests/host', '-IInc', '-IThirdParty/FreeModbus/include',
                        str(path / 'state_test.c'), 'Src/encoder_calibration.c',
                        'Src/modbus_registers.c', 'Src/snapshot_registers.c',
                        '-lm', '-o', str(path / 'state_test')], cwd=ROOT, check=True)
        subprocess.run([str(path / 'state_test')], check=True)


if __name__ == '__main__':
    main()
