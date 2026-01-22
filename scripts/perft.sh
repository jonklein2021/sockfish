#!/bin/bash
set -uo pipefail

NUM_TESTS=128
RESULTS_FILE="../scripts/results.txt"
declare -a failed

SECONDS=0

cd ../build || exit 1

if ! meson compile perft; then
    echo "Build failed" >&2
    exit 1
fi

: > "$RESULTS_FILE"

for ((i = 1; i <= NUM_TESTS; ++i)); do
    echo "Running test $i..."

    ./perft "$i" >> "$RESULTS_FILE"
    status=$?

    if (( status != 0 )); then
        failed+=("$i")

        if (( status >= 128 )); then
            signal=$((status - 128))
            echo "⚠️  Test $i crashed (signal $signal)" >> "$RESULTS_FILE"
        else
            echo "⚠️  Test $i exited with status $status" >> "$RESULTS_FILE"
        fi
    fi
done

if [[ ${#failed[@]} -ne 0 ]]; then
    echo
    echo "Failed tests:"
    for failed_test in "${failed[@]}"; do
        echo "  Test $failed_test"
    done
else
    echo
    echo "All tests passed ✅"
fi

elapsed=$SECONDS
printf "\nTotal execution time: %02d:%02d\n" $((elapsed/60)) $((elapsed%60))

cd ../scripts

