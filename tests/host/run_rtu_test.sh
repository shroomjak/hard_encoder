#!/bin/sh
set -eu
cd "$(dirname "$0")/../.."
out=$(mktemp)
trap 'rm -f "$out"' EXIT
cc -std=c99 -Itests/host -IInc -IThirdParty/FreeModbus/include \
   -IThirdParty/FreeModbus/rtu \
   Src/snapshot_registers.c Src/modbus_registers.c \
   ThirdParty/FreeModbus/mb.c ThirdParty/FreeModbus/rtu/mbrtu.c \
   ThirdParty/FreeModbus/rtu/mbcrc.c \
   ThirdParty/FreeModbus/functions/mbfuncholding.c \
   ThirdParty/FreeModbus/functions/mbfuncinput.c \
   ThirdParty/FreeModbus/functions/mbutils.c tests/host/rtu_test.c -o "$out"
"$out"
