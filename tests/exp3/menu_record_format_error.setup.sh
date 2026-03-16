#!/usr/bin/env bash
set -euo pipefail
bash "$(dirname "$0")/helpers/reset_card_file.sh"
printf 'broken|record\n' > data/cards.txt
