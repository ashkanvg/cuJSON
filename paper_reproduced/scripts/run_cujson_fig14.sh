#!/bin/bash
set -e

# ------------------------------
# Step 1: Paths and Setup
# ------------------------------
mkdir -p results
mkdir -p cujson_results

SRC="../src/reproduced/cuJSON-standardjson-parser-output.cu"
BINARY="./cujson_results/output_large.exe"
OUT_FILE="results/cujson_fig14.csv"

: > "$OUT_FILE"
echo "Dataset,MaxOutputSize(MB)" > "$OUT_FILE"

ORDERED_KEYS=("TT" "BB" "GMD" "NSPL" "WM" "WP")

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
nvcc -O3 -o "$BINARY" "$SRC" -w -gencode=arch=compute_89,code=sm_89

# ------------------------------
# Step 3: Run and Extract Output Size
# ------------------------------
echo "🚀 Measuring cuJSON Output Size..."

for label in "${ORDERED_KEYS[@]}"; do
    JSON_PATH="${DATASETS[$label]}"
    echo "📂 $label: $JSON_PATH"

    max_size=0
    for i in {1..10}; do
        OUTPUT=$("$BINARY" -b "$JSON_PATH")

        # Extract output size from cuJSON stdout
        PARSER_MB=$(echo "$OUTPUT" | grep -oP "Parser's Output Size:\s*\K[0-9]+(\.[0-9]+)?")

        # Compute input .json file size in MB
        INPUT_BYTES=$(stat -c%s "$JSON_PATH")
        INPUT_MB=$(awk "BEGIN {print $INPUT_BYTES / 1024 / 1024}")

        # Validate extraction
        if [[ -z "$PARSER_MB" ]]; then
            echo "❌ Failed to extract parser size for $label run $i"
            echo "$OUTPUT"
            exit 1
        fi

        # Compute total output size
        TOTAL_MB=$(awk "BEGIN {print $PARSER_MB + $INPUT_MB}")
        echo "Run $i total output size: $TOTAL_MB MB"

        # Track max over 10 runs
        max_size=$(awk "BEGIN {print ($TOTAL_MB > $max_size) ? $TOTAL_MB : $max_size}")
    done

    echo "$label,$max_size" >> "$OUT_FILE"
    echo "✅ $label max output size: $max_size MB"
done

echo "✅ cuJSON output sizes saved to $OUT_FILE"
