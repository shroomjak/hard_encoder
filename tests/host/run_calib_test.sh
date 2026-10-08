#!/bin/sh
# Командный канал самокалибровки: защита запуска, откат, живое зеркало
# переменных main.c и постраничная выгрузка таблиц.
set -eu
cd "$(dirname "$0")/../.."
out=$(mktemp)
trap 'rm -f "$out"' EXIT
cc -std=c99 -Wall -Wextra -Werror -Itests/host -IInc \
    -IThirdParty/FreeModbus/include \
    Src/snapshot_registers.c Src/modbus_registers.c Src/modbus_calib.c \
    tests/host/encoder_vars_stub.c tests/host/calib_test.c -lm -o "$out"
"$out"
