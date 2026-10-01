#!/usr/bin/env bash
#
# Хостовые проверки протокола RS-485 (железо не требуется).
#
#   ./tools/run_tests.sh
#
# Что проверяется:
#   1) Inc/rs485_proto.h и master_esp32/rs485_proto.h побайтово одинаковы
#      (Arduino IDE видит заголовки только в каталоге скетча, поэтому копия
#      неизбежна - зато расхождение ловится здесь, а не на стенде);
#   2) crc_table[] в Src/main.c - это именно CRC-8/MAXIM, то есть кодек и
#      прошивка считают одну и ту же контрольную сумму;
#   3) кодек собирается без единого предупреждения и как C, и как C++
#      (прошивка - C, скетч ESP32 - C++);
#   4) юнит-тесты кодека проходят.

set -euo pipefail

cd "$(dirname "$0")/.."

ok()   { printf '  \033[32mok\033[0m   %s\n' "$1"; }
fail() { printf '  \033[31mFAIL\033[0m %s\n' "$1"; exit 1; }

echo "== 1. копии rs485_proto.h =="
# Канонический файл - Inc/rs485_proto.h (CP1251, как все *.h проекта).
# Копия для Arduino - master_esp32/rs485_proto.h (UTF-8, как весь скетч).
# Сравниваем содержимое с точностью до кодировки и перевода строки.
if diff -q <(iconv -f CP1251 -t UTF-8 Inc/rs485_proto.h | tr -d '\r') \
           <(tr -d '\r' < master_esp32/rs485_proto.h) >/dev/null; then
    ok "Inc/rs485_proto.h == master_esp32/rs485_proto.h (с точностью до кодировки)"
else
    fail "копии rs485_proto.h разошлись - выполните tools/sync_proto_header.sh"
fi

echo "== 2. crc_table[] в Src/main.c == CRC-8/MAXIM =="
python3 - <<'PY'
import re, sys
src = open('Src/main.c', 'rb').read().decode('cp1251', errors='replace')
m = re.search(r'const unsigned char crc_table\[256\]\s*=\s*\{(.*?)\}', src, re.S)
if not m:
    print('  не найдена crc_table[] в Src/main.c'); sys.exit(1)
vals = [int(x, 16) for x in re.findall(r'0x([0-9A-Fa-f]{2})', m.group(1))]
ref = []
for i in range(256):
    c = i
    for _ in range(8):
        c = (c >> 1) ^ 0x8C if c & 1 else c >> 1
    ref.append(c)
if len(vals) != 256 or vals != ref:
    print('  crc_table[] НЕ совпадает с CRC-8/MAXIM (poly 0x31, refl 0x8C)'); sys.exit(1)
PY
ok "полином совпадает (poly 0x31, reflected 0x8C, init 0x00)"

echo "== 3+4. сборка и запуск тестов =="
tmp="$(mktemp -d)"
trap 'rm -rf "$tmp"' EXIT

CFLAGS="-std=c99 -Wall -Wextra -Werror -Wshadow -Wconversion -Wsign-conversion -O2 -IInc -finput-charset=CP1251 -fexec-charset=UTF-8"
# shellcheck disable=SC2086
gcc $CFLAGS tools/test_rs485_proto.c -o "$tmp/test_c"
ok "gcc -std=c99 -Wall -Wextra -Werror (как в прошивке)"

CXXFLAGS="-std=c++11 -Wall -Wextra -Werror -Wshadow -O2 -IInc -finput-charset=CP1251 -fexec-charset=UTF-8 -x c++"
# shellcheck disable=SC2086
g++ $CXXFLAGS tools/test_rs485_proto.c -o "$tmp/test_cpp"
ok "g++ -std=c++11 -Wall -Wextra -Werror (как в скетче ESP32)"

"$tmp/test_c"
"$tmp/test_cpp" >/dev/null
ok "юнит-тесты кодека"

echo
echo "все проверки пройдены"
