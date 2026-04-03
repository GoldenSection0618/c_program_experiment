#!/usr/bin/env bash
set -euo pipefail

older_start_time="$(date -d '120 seconds ago' '+%Y-%m-%d %H:%M:%S')"
latest_start_time="$(date -d '20 seconds ago' '+%Y-%m-%d %H:%M:%S')"

mkdir -p data
cat > data/cards.txt <<'DATA'
stopA6|pwd001|1|2026-01-01 00:00:00|2027-01-01 00:00:00|0|2026-01-01 00:00:00|0|10000|0
DATA
cat > data/billings.txt <<DATA
stopA6|${older_start_time}|0|0|0|0
stopA6|${latest_start_time}|0|0|0|0
DATA
