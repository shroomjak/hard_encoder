#!/bin/sh
# Хостовая проверка: поиск, выбор ID и автомат poll/calib на ESP32 (UTF-8).
# Из корня репозитория: sh tests/host/run_workflow_test.sh
# Требуются POSIX sh, cc (C99) и mktemp; на Windows удобно запускать в WSL.
# Сборка идёт во временный файл, удаляемый при выходе; прошивки не создаются.
# set -e передаёт ненулевой код сборки/теста вызывающему процессу.
# Подробные сценарии и ограничения — в преамбуле workflow_test.c.
set -eu
cd "$(dirname "$0")/../.."
out=$(mktemp)
trap 'rm -f "$out"' EXIT
cc -std=c99 -Wall -Wextra -Werror -IInc -Iesp32_master/main \
    esp32_master/main/workflow.c tests/host/workflow_test.c -o "$out"
"$out"
