#!/bin/sh
# Хостовая проверка: SNAP/STATUS/DATA и масштаб угла (UTF-8).
# Из корня репозитория: sh tests/host/run_snapshot_test.sh
# Требуются POSIX sh, cc (C99) и mktemp; на Windows удобно запускать в WSL.
# Сборка идёт во временный файл, удаляемый при выходе; прошивки не создаются.
# set -e передаёт ненулевой код сборки/теста вызывающему процессу.
# Подробные сценарии и ограничения — в преамбуле snapshot_test.c.
set -eu
cd "$(dirname "$0")/../.."
out=$(mktemp)
trap 'rm -f "$out"' EXIT
cc -std=c99 -Wall -Wextra -Werror -Itests/host -IInc \
    -IThirdParty/FreeModbus/include \
    Src/snapshot_registers.c Src/modbus_registers.c Src/encoder_calibration.c tests/host/calibration_stubs.c tests/host/snapshot_test.c -o "$out"
"$out"
