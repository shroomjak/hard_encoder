#!/bin/sh
set -eu
cd "$(dirname "$0")/../.."
out=$(mktemp)
trap 'rm -f "$out"' EXIT
cc -std=c99 -Wall -Wextra -Werror -Itests/host -IInc \
    -IThirdParty/FreeModbus/include \
    Src/snapshot_registers.c Src/modbus_registers.c Src/modbus_calib.c \
   tests/host/encoder_vars_stub.c tests/host/snapshot_test.c -o "$out"
"$out"
