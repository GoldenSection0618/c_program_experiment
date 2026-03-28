#!/usr/bin/env bash
set -euo pipefail

mkdir -p data
cat > data/cards.txt <<'EOF'
startA1|pwd001|0|2026-01-01 00:00:00|2027-01-01 00:00:00|0|2026-01-01 00:00:00|0|10000|0
EOF
rm -f data/billings.txt
