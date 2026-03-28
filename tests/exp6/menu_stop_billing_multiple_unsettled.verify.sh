#!/usr/bin/env bash
set -euo pipefail

if ! awk -F'|' 'BEGIN{old_ok=0; latest_ok=0} $1=="stopA6" && $2=="2026-03-27 10:00:00" && $3=="0" && $5=="0" {old_ok=1} $1=="stopA6" && $2=="2026-03-27 12:00:00" && $3!="0" && $5=="1" {latest_ok=1} END {exit (old_ok && latest_ok) ? 0 : 1}' data/billings.txt; then
    echo "[verify] multiple-unsettled selection rule failed" >&2
    exit 1
fi
