#!/usr/bin/env bash
set -euo pipefail

mkdir -p data
cat > data/cards.txt <<'DATA'
stopA4|pwd001|1|2026-01-01 00:00:00|2027-01-01 00:00:00|0|2026-01-01 00:00:00|0|10000|0
DATA
cat > data/billings.txt <<'DATA'
stopA4|2026-03-27 10:00:00|0|0|0|0
DATA
