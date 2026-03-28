#!/usr/bin/env bash

set -euo pipefail

if [ "$#" -ne 4 ]; then
    echo "Usage: $0 <binary> <input_file> <expect_file> <output_file>" >&2
    exit 2
fi

binary="$1"
input_file="$2"
expect_file="$3"
output_file="$4"
suite_base="${input_file%.input}"
setup_script="${suite_base}.setup.sh"
verify_script="${suite_base}.verify.sh"
backup_state_dir="build/test_state"
backup_name="$(basename "$suite_base")"
backup_file="${backup_state_dir}/${backup_name}.card.bin.bak"
missing_marker="${backup_state_dir}/${backup_name}.card.bin.missing"

restore_cards_file() {
    if [ -f "$backup_file" ]; then
        mkdir -p "$(dirname data/card.bin)"
        mv "$backup_file" data/card.bin
    elif [ -f "$missing_marker" ]; then
        rm -f data/card.bin
    fi

    rm -f "$backup_file" "$missing_marker"
}

cleanup() {
    restore_cards_file
}

prepare_backup_state() {
    mkdir -p "$backup_state_dir"
    rm -f "$backup_file" "$missing_marker"

    if [ -f data/card.bin ]; then
        cp data/card.bin "$backup_file"
    else
        : > "$missing_marker"
    fi
}

trap cleanup EXIT

if [ ! -f "$binary" ]; then
    echo "[test] error: binary not found: $binary" >&2
    exit 1
fi

if [ ! -f "$input_file" ]; then
    echo "[test] error: input file not found: $input_file" >&2
    exit 1
fi

if [ ! -f "$expect_file" ]; then
    echo "[test] error: expect file not found: $expect_file" >&2
    exit 1
fi

mkdir -p "$(dirname "$output_file")"
prepare_backup_state

if [ -f "$setup_script" ]; then
    bash "$setup_script" "$binary" "$input_file" "$expect_file" "$output_file"
fi

"$binary" < "$input_file" > "$output_file"

failed=0
line_no=0

while IFS=$'\t' read -r expected pattern || [ -n "${expected}${pattern}" ]; do
    line_no=$((line_no + 1))

    if [ -z "${expected}" ] && [ -z "${pattern}" ]; then
        continue
    fi

    if [[ "${expected}" == \#* ]]; then
        continue
    fi

    if ! [[ "${expected}" =~ ^[0-9]+$ ]]; then
        echo "[test] error: invalid expect format at line ${line_no}: ${expected}" >&2
        exit 1
    fi

    actual="$(grep -cF -- "${pattern}" "${output_file}" || true)"
    if [ "${actual}" -ne "${expected}" ]; then
        echo "[test] mismatch at line ${line_no}: expected=${expected}, actual=${actual}, pattern=${pattern}" >&2
        failed=1
    fi
done < "$expect_file"

if [ "${failed}" -ne 0 ]; then
    echo "[test] failed. output: ${output_file}" >&2
    exit 1
fi

if [ -f "$verify_script" ]; then
    bash "$verify_script" "$binary" "$input_file" "$expect_file" "$output_file"
fi

echo "[test] passed (${input_file} -> ${output_file})"
