#!/usr/bin/env bash
set -euo pipefail

if [ -f data/card.bin ] && [ -s data/card.bin ]; then
    echo "[verify] invalid card name should not persist data" >&2
    exit 1
fi
