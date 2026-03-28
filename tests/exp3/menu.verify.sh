#!/usr/bin/env bash
set -euo pipefail

gcc -std=c11 -Wall -Wextra -pedantic -Iinclude \
    tests/exp3/helpers/card_bin_check.c src/data/repository.c \
    -o build/bin/card_bin_check

if [ ! -f data/card.bin ]; then
    echo "[verify] card.bin not created" >&2
    exit 1
fi

build/bin/card_bin_check 1 card001 pass01 0 0 0 10000
