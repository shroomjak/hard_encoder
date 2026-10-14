#!/bin/sh
set -eu
cd "$(dirname "$0")/../.."
out=$(mktemp)
trap 'rm -f "$out"' EXIT
cc -std=c99 -Wall -Wextra -Werror -IInc -Iesp32_master/main \
    esp32_master/main/workflow.c tests/host/workflow_test.c -o "$out"
"$out"
