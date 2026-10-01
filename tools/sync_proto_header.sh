#!/usr/bin/env bash
#
# Пересобрать копию кодека для скетча ESP32 из канонического заголовка.
#
#   Inc/rs485_proto.h            - канон, CP1251 (как все *.h проекта, IAR)
#   master_esp32/rs485_proto.h   - копия,  UTF-8  (как весь скетч, Arduino IDE)
#
# Любые правки протокола вносятся ТОЛЬКО в Inc/rs485_proto.h, после чего
# запускается этот скрипт. Расхождение копий ловит tools/run_tests.sh.

set -euo pipefail
cd "$(dirname "$0")/.."

iconv -f CP1251 -t UTF-8 Inc/rs485_proto.h > master_esp32/rs485_proto.h
echo "master_esp32/rs485_proto.h обновлён из Inc/rs485_proto.h"
