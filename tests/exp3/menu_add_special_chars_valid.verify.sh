#!/usr/bin/env bash
set -euo pipefail

gcc -std=c11 -Wall -Wextra -pedantic -Iinclude \
    tests/exp3/helpers/card_bin_check.c src/data/repository.c \
    -o build/bin/card_bin_check

build/bin/card_bin_check 1 card_001@ p_1! 0 0 0 10000
