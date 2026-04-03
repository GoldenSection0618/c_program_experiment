#!/usr/bin/env bash
set -euo pipefail

start_time="$(date -d '6 minutes ago' '+%Y-%m-%d %H:%M:%S')"

mkdir -p data
cat > data/cards.txt <<'DATA'
stopA3|pwd001|1|2026-01-01 00:00:00|2027-01-01 00:00:00|0|2026-01-01 00:00:00|0|5|0
DATA
cat > data/billings.txt <<DATA
stopA3|${start_time}|0|0|0|0
DATA
