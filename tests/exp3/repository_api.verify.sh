#!/usr/bin/env bash
set -euo pipefail

gcc -std=c11 -Wall -Wextra -pedantic -Iinclude \
    tests/exp3/helpers/repository_api_check.c src/data/repository.c \
    -o build/bin/repository_api_check

build/bin/repository_api_check

gcc -std=c11 -Wall -Wextra -pedantic -Iinclude \
    tests/exp3/helpers/card_bin_check.c src/data/repository.c \
    -o build/bin/card_bin_check

build/bin/card_bin_check 1 cardApi pwd001 0 500 2 20500
