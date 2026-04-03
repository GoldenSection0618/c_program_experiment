#!/usr/bin/env bash
set -euo pipefail

start_time="$(date -d '20 seconds ago' '+%Y-%m-%d %H:%M:%S')"

mkdir -p data
cat > data/cards.txt <<'DATA'
stopA7|pwd001|1|2026-01-01 00:00:00|2027-01-01 00:00:00|0|2026-01-01 00:00:00|0|10000|0
DATA
cat > data/billings.txt <<DATA
stopA7|${start_time}|0|0|0|0
DATA
chmod 444 data/billings.txt
