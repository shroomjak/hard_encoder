#!/bin/sh
# Хостовая проверка: путь RTU-кадр → FreeModbus → callback → ответ (UTF-8).
# Из корня репозитория: sh tests/host/run_rtu_test.sh
# Требуются POSIX sh, cc (C99) и mktemp; на Windows удобно запускать в WSL.
# Сборка идёт во временный файл, удаляемый при выходе; прошивки не создаются.
# set -e передаёт ненулевой код сборки/теста вызывающему процессу.
# Подробные сценарии и ограничения — в преамбуле rtu_test.c.
set -eu
cd "$(dirname "$0")/../.."
out=$(mktemp)
trap 'rm -f "$out"' EXIT
cc -std=c99 -Itests/host -IInc -IThirdParty/FreeModbus/include \
   -IThirdParty/FreeModbus/rtu \
   Src/snapshot_registers.c Src/modbus_registers.c Src/encoder_calibration.c tests/host/calibration_stubs.c \
   ThirdParty/FreeModbus/mb.c ThirdParty/FreeModbus/rtu/mbrtu.c \
   ThirdParty/FreeModbus/rtu/mbcrc.c \
   ThirdParty/FreeModbus/functions/mbfuncholding.c \
   ThirdParty/FreeModbus/functions/mbfuncinput.c \
   ThirdParty/FreeModbus/functions/mbutils.c tests/host/rtu_test.c -o "$out"
"$out"
