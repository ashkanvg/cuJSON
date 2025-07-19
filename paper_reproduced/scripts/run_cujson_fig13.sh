#!/bin/bash
set -e

echo "🚀 Benchmarking time breakdown (Fig. 13)..."

SRC="../src/reproduced/cuJSON-standardjson-breakdown.cu"
BINARY="./cujson_results/cujson_breakdown.exe"

# ------------------------------
# Step 2: Compile cuJSON
# ------------------------------
echo "🔧 Compiling cuJSON..."
nvcc -O3 -o "$BINARY" "$SRC" -w -gencode=arch=compute_61,code=sm_61

# Output paths
mkdir -p results
OUT_FILE="results/fig13_data.csv"
: > "$OUT_FILE"



# Write header
echo "Dataset,h2d,validation,tokenization,parsing,d2h" > "$OUT_FILE"

# Ordered datasets
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

# Path to your binary that reports breakdown
BINARY="./cujson_results/cujson_breakdown.exe"

# Loop through datasets
for label in "${ORDERED_KEYS[@]}"; do
    echo "📂 Running on $label"

    JSON_PATH="${DATASETS[$label]}"
    sum=(0 0 0 0 0)

    for i in {1..10}; do
        LINE=$("$BINARY" -b "$JSON_PATH")
        IFS=',' read -ra fields <<< "$LINE"
        for j in {0..4}; do
            sum[$j]=$(awk "BEGIN {print ${sum[$j]} + ${fields[$j]}}")
        done
    done

    # Compute averages
    avg=()
    for j in {0..4}; do
        avg[$j]=$(awk "BEGIN {print ${sum[$j]} / 10}")
    done

    # Save to CSV
    echo "$label,${avg[0]},${avg[1]},${avg[2]},${avg[3]},${avg[4]}" >> "$OUT_FILE"
done

echo "✅ Time breakdown saved to $OUT_FILE"
