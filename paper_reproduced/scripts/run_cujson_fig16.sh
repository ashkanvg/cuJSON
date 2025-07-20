#!/bin/bash
set -e

# ------------------------------
# Step 1: Setup
# ------------------------------
mkdir -p results
mkdir -p cujson_results

SRC="../src/reproduced/cuJSON-jsonlines-total-parsing.cu"
BINARY="./cujson_results/output_large.exe"
OUT_FILE="results/fig16_data.csv"

ORDERED_KEYS=("tt" "bb" "gmp" "nspl" "wm" "wp")
SIZES_MB=(2 4 8 16 32 64 128 256)

BASE_DIR="../../dataset/scalability"

# ------------------------------
# Step 2: Compile
# ------------------------------
echo "🔧 Compiling cuJSON..."
nvcc -O3 -o "$BINARY" "$SRC" -w -gencode=arch=compute_89,code=sm_89

# ------------------------------
# Step 3: Run and collect results
# ------------------------------
echo "🚀 Benchmarking scalability (cuJSON parsing time)..."

# Header
echo -n "Dataset" > "$OUT_FILE"
for size in "${SIZES_MB[@]}"; do
    echo -n ",$size" >> "$OUT_FILE"
done
echo "" >> "$OUT_FILE"

# Per dataset
for label in "${ORDERED_KEYS[@]}"; do
    echo -n "$label" >> "$OUT_FILE"

    for size in "${SIZES_MB[@]}"; do
        JSON_PATH="$BASE_DIR/$label/output_${size}MB.json"

        if [[ ! -f "$JSON_PATH" ]]; then
            echo -n ",NaN" >> "$OUT_FILE"
            continue
        fi

        SUM=0
        for i in {1..10}; do
            # OUTPUT=$("$BINARY" -b "$JSON_PATH")
            PARSE_TIME=$("$BINARY" -b "$JSON_PATH")
            if [[ -z "$PARSE_TIME" ]]; then
                echo "⚠️ Failed to extract time for $label - ${size}MB (run $i)"
                continue
            fi

            SUM=$(awk "BEGIN {print $SUM + $PARSE_TIME}")
        done

        AVG=$(awk "BEGIN {print $SUM / 10}")
        echo -n ",$AVG" >> "$OUT_FILE"
    done

    echo "" >> "$OUT_FILE"
done

echo "✅ Scalability parsing times saved to $OUT_FILE"
