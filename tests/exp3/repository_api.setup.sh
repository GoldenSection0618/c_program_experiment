#!/usr/bin/env bash
set -euo pipefail

bash "$(dirname "$0")/helpers/reset_card_file.sh"

gcc -std=c11 -Wall -Wextra -pedantic -Iinclude \
    tests/exp3/helpers/card_bin_write.c src/data/repository.c \
    -o build/bin/card_bin_write

build/bin/card_bin_write cardApi pwd001 0 0 0 20000
