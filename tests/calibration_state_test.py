#!/usr/bin/env python3
"""Команда Modbus → реальные алгоритмы main.c → LIVE-состояние (без плат).

Из CP1251 Src/main.c извлекаются angtab_cal/offset_cal/angk_cal, CRC16 и
объявления их переменных. Линкуются реальные encoder_calibration.c и callbacks
Modbus. Тест подаёт синтетические измерения и подменяет только GPIO/I2C.
Состояния done НЕ подставляются тестом: их должны выставить сами алгоритмы.
Это проверка переходов/связки команд, не точности оптики или реальной EEPROM.
Запуск: python3 tests/calibration_state_test.py (Python 3, cc C99).
"""
from pathlib import Path
import os
import re
import shlex
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]


def function(source, signature):
    start = source.index(signature)
    pos = source.index('{', start)
    depth = 0
    for end in range(pos, len(source)):
        depth += (source[end] == '{') - (source[end] == '}')
        if depth == 0:
            return source[start:end + 1]
    raise ValueError(signature)


def main():
    source = (ROOT / 'Src/main.c').read_text(encoding='cp1251')
    # Strip comments before extraction; braces in documentation are not C syntax.
    source = re.sub(r'/\*.*?\*/|//[^\n]*', '', source, flags=re.S)
    defines = '\n'.join(re.findall(
        r'^#define\s+(?:BIT_TAB_SIZE|EEPROM_DEVICE_PAGE\d|eeprom_\w+)\b[^\n]*',
        source, flags=re.M))
    variables = source[source.index('int pix_dif_num_avg='):source.index('unsigned char encoder_state=')]
    pixels = source[source.index('float buf_x3[128]'):source.index('float r_angle;')]
    crc_table = re.search(r'static\s+unsigned int crc_16_table\[16\].*?;', source, re.S).group()
    functions = '\n'.join(function(source, sig) for sig in [
        'unsigned short crc_16_step(', 'void angtab_cal(', 'void offset_cal(', 'void angk_cal('])
    harness = (ROOT / 'tests/host/calibration_state_harness.c').read_text()
    generated = harness.replace('/* EXTRACTED_DECLARATIONS */', defines + '\n' + variables + '\n' + pixels + '\n' + crc_table)
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
