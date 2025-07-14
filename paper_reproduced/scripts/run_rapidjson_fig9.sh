#!/bin/bash
set -e

# ------------------------------
# Step 1: Setup
# ------------------------------
echo "🔧 Compiling RapidJSON benchmarks..."
mkdir -p results

cd ../related_works/rapidjson/  # <--- Adjust this path if needed

# make clean
# make all

TMP_FILE="../../scripts/results/rapidjson_fig9_tmp.csv"
: > "$TMP_FILE"

ORDERED_KEYS=("TT" "BB" "GMD" "NSPL" "WM" "WP")
declare -A BINARIES=(
    ["TT"]="output-twitter.exe"
    ["BB"]="output-bestbuy.exe"
    ["GMD"]="output-google.exe"
    ["NSPL"]="output-nspl.exe"
    ["WM"]="output-walmart.exe"
    ["WP"]="output-wiki.exe"
)

# ------------------------------
# Step 2: Run each binary 10 times
# ------------------------------
echo "🚀 Benchmarking RapidJSON parsers..."
for key in "${ORDERED_KEYS[@]}"; do
    BIN="${BINARIES[$key]}"
    echo "📂 $key: $BIN"

    SUM=0
    for i in {1..10}; do
        TIME=$(./"$BIN" | grep -Eo '[0-9]+(\.[0-9]+)?' | tail -1)
        SUM=$(awk "BEGIN {print $SUM+$TIME}")
    done
    AVG=$(awk "BEGIN {print $SUM/10}")
    echo "$key,$AVG" >> "$TMP_FILE"
done

echo "✅ RapidJSON results saved to $TMP_FILE"
