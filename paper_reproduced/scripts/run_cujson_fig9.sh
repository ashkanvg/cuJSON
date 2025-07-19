#!/bin/bash
set -e

# ------------------------------
# Step 1: Paths and Setup
# ------------------------------
mkdir -p results
mkdir -p cujson_results

SRC="../src/reproduced/cuJSON-standardjson-total-parsing.cu"
BINARY="./cujson_results/output_large.exe"
TMP_FILE="results/cujson_fig9.csv"
OUT_FILE="results/fig9_data.csv"
: > "$TMP_FILE"

# Ordered list of dataset keys
ORDERED_KEYS=("TT" "BB" "GMD" "NSPL" "WM" "WP")

# Map keys to file paths
declare -A DATASETS=(
    ["TT"]="../../dataset/twitter_large_record.json"
    ["BB"]="../../dataset/bestbuy_large_record.json"
    ["GMD"]="../../dataset/google_map_large_record.json"
    ["NSPL"]="../../dataset/nspl_large_record.json"
    ["WM"]="../../dataset/walmart_large_record.json"
    ["WP"]="../../dataset/wiki_large_record.json"
)

# ------------------------------
# Step 2: Compile cuJSON
# ------------------------------
echo "🔧 Compiling cuJSON..."
nvcc -O3 -o "$BINARY" "$SRC" -w -gencode=arch=compute_61,code=sm_61

# ------------------------------
# Step 3: Run on each dataset in specified order
# ------------------------------
echo "🚀 Benchmarking cuJSON..."
for label in "${ORDERED_KEYS[@]}"; do
    JSON_PATH="${DATASETS[$label]}"
    echo "📂 $label: $JSON_PATH"

    SUM=0
    for i in {1..10}; do
        TIME=$("$BINARY" -b "$JSON_PATH" | grep -Eo '[0-9]+(\.[0-9]+)?' | tail -1)
        SUM=$(awk "BEGIN {print $SUM+$TIME}")
    done
    AVG=$(awk "BEGIN {print $SUM/10}")
    echo "$label,$AVG" >> "$TMP_FILE"
done

# ------------------------------
# Step 4: Format CSV
# ------------------------------
# Output CSV: header + cuJSON row
# echo "Dataset,cuJSON" > "$OUT_FILE"
# cat "$TMP_FILE" >> "$OUT_FILE"

echo "✅ cuJSON results saved to $TMP_FILE"
