#!/bin/sh
# Хостовой тест двух режимов ESP32 master (без ESP-IDF, esp-modbus и сети).
#   1. рабочий режим: цикл SNAP/STATUS/READ не пишет CAL_CMD и CAL_PAGE;
#   2. калибровка: порядок команд, адрес головки, выгрузка ang_tab, ввод оператора.
# Заглушки ESP-IDF — tests/host/esp32/stubs. Значения CONFIG_* здесь заданы
# явно, как их выбрал бы menuconfig.
set -eu
here=$(cd "$(dirname "$0")" && pwd)
root=$(cd "$here/../.." && pwd)
main="$root/esp32_master/main"
src="$here/esp32/test_modes.c"
stubs="$here/esp32/stubs"
out=$(mktemp -d)
trap 'rm -rf "$out"' EXIT
CFLAGS="-std=c99 -Wall -Wextra -Werror -I $stubs -I $main"

echo "== рабочий режим (ENCODER_MODE_POLL)"
gcc $CFLAGS "$src" "$main/poll_mode.c" -o "$out/poll"
"$out/poll" </dev/null

calib() {
    name=$1; shift
    echo "== калибровка: $name"
    gcc $CFLAGS -DTEST_CALIB=1 "$@" "$src" "$main/calib_mode.c" -o "$out/calib"
}

# A: по умолчанию — подтверждение выключено, дамп до и после, один круг
calib "дамп, без подтверждения, один круг" \
    -DCONFIG_ENCODER_MODE_CALIB=1 -DCONFIG_ENCODER_CALIB_SLAVE=2 \
    -DCONFIG_ENCODER_CALIB_CONFIRM=0 -DCONFIG_ENCODER_CALIB_DUMP=1 \
    -DCONFIG_ENCODER_CALIB_TWICE=0 -DCONFIG_ENCODER_CALIB_POLL_MS=500 \
    '-DEXPECT_CMDS=0x14,0x13,0x11,0x12,0x10' -DEXPECT_PAGES=18
"$out/calib" </dev/null > "$out/calib_a.txt"
# CSV ang_tab: по 144 строки до и после, вывод пригоден для diff
test "$(grep -c '^[0-9]*,' "$out/calib_a.txt")" -eq 288
grep -q '^# ang_tab (градусы), головка 2, до калибровки$' "$out/calib_a.txt"
grep -q '^143,143.5$' "$out/calib_a.txt"
tail -n 3 "$out/calib_a.txt"

# B: второй круг, без дампа. Булевы опции со значением n в sdkconfig.h не
# определены вовсе — собираем так же, как это делает ESP-IDF.
calib "второй круг, без дампа и подтверждения (опции не определены)" \
    -DCONFIG_ENCODER_MODE_CALIB=1 -DCONFIG_ENCODER_CALIB_SLAVE=1 \
    -DCONFIG_ENCODER_CALIB_TWICE=1 -DCONFIG_ENCODER_CALIB_POLL_MS=500 \
    '-DEXPECT_CMDS=0x14,0x13,0x11,0x12,0x14,0x13,0x10' -DEXPECT_PAGES=0
"$out/calib" </dev/null > /dev/null

# C: с подтверждением; оператор нажимает Enter на BUFK и q на OFFSET → только 0x14, затем 0x10
printf '\nq\n' > "$out/input"
calib "подтверждение и q на втором шаге" \
    -DCONFIG_ENCODER_MODE_CALIB=1 -DCONFIG_ENCODER_CALIB_SLAVE=1 \
    -DCONFIG_ENCODER_CALIB_CONFIRM=1 -DCONFIG_ENCODER_CALIB_POLL_MS=500 \
    '-DEXPECT_CMDS=0x14,0x10' -DEXPECT_PAGES=0
"$out/calib" < "$out/input" > "$out/calib_c.txt"
grep -q 'прервано — отправляю 0x10' "$out/calib_c.txt"

echo "все проверки режимов ESP32 пройдены"
