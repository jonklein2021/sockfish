#!/bin/bash
set -uo pipefail

NUM_TESTS=128
RESULTS_FILE="results"
declare -a failed

# Start timer
SECONDS=0

# Build project
cd build
if ! meson compile; then
    echo "Build failed" >&2
    exit 1
fi

# Clear output file
: > "$RESULTS_FILE"

# Run perft tests
for ((i = 1; i <= NUM_TESTS; ++i)); do
    echo "Running test $i..."

    ./perft "$i" >> "$RESULTS_FILE"
    status=$?

    if [[ $status -ne 0 ]]; then
        failed += $i
    fi
done

# Report errors
if [[${#failed[@]} -ne 0]] then
    echo "Failed tests: "
    for failed_test in "${failed[@]}"; do
        echo "Test $item"
    done
fi

# End timer
elapsed=$SECONDS
printf "\nTotal execution time: %02d:%02d\n" $((elapsed/60)) $((elapsed%60))
cd ..
