#!/bin/bash
set -e

# -------------------------------
# Environment Setup
# -------------------------------

echo "🔧 Compiling Pison (with gcc-toolset)..."
mkdir -p results

# Compile inside GCC 13 environment
cd ../related_works/pison/query && make clean && make all

# Move into bin after compilation
cd bin

TMP_FILE="../../../../scripts/results/pison_fig15.csv"
: > "$TMP_FILE"

ORDERED_KEYS=("TT" "BB" "GMD" "NSPL" "WM" "WP")
declare -A BINARIES=(
    ["TT"]="twitter"
    ["BB"]="bestbuy"
    ["GMD"]="google"
    ["NSPL"]="nspl"
    ["WM"]="walmart"
    ["WP"]="wiki"
)

echo "🚀 Benchmarking Pison parsers..."
for key in "${ORDERED_KEYS[@]}"; do
    BIN="${BINARIES[$key]}"
    echo "📂 $key: $BIN"

    SUM=0
    for i in {1..10}; do
        TIME=$(./"$BIN" | tail -n 1)
        SUM=$(awk "BEGIN {print $SUM+$TIME}")
    done
    AVG=$(awk "BEGIN {print $SUM/10}")
    echo "$key,$AVG" >> "$TMP_FILE"
done

echo "✅ Pison results saved to $TMP_FILE"




# Now compute the overall average of all AVG values and store it in a separate file
TOTAL_SUM=0
TOTAL_COUNT=0
while IFS=',' read -r method avg; do
    TOTAL_SUM=$(awk "BEGIN {print $TOTAL_SUM+$avg}")
    TOTAL_COUNT=$((TOTAL_COUNT + 1))
done < "$TMP_FILE"

OVERALL_AVG=$(awk "BEGIN {print $TOTAL_SUM/$TOTAL_COUNT}")
echo "AVERAGE,$OVERALL_AVG" >> "$TMP_FILE"

echo "✅ Pison results (with overall average) saved to $TMP_FILE"