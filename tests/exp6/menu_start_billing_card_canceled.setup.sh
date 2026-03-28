#!/usr/bin/env bash
set -euo pipefail

mkdir -p data
cat > data/cards.txt <<'DATA'
startA5|pwd001|2|2026-01-01 00:00:00|2027-01-01 00:00:00|0|2026-01-01 00:00:00|0|10000|0
DATA
rm -f data/billings.txt
