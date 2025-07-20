#!/bin/bash
set -e

# ------------------------------
# Step 1: Paths and Setup
# ------------------------------
mkdir -p results
mkdir -p cujson_results

SRC="../src/reproduced/cuJSON-jsonlines-total-parsing.cu"
BINARY="./cujson_results/output_large.exe"
OUT_FILE="results/cujson_fig12.csv"

: > "$OUT_FILE"
echo "Dataset,PeakMemory(MiB)" > "$OUT_FILE"

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
# Step 3: Run & Track Peak GPU Memory
# ------------------------------
echo "🚀 Measuring peak GPU memory usage..."

for label in "${ORDERED_KEYS[@]}"; do
    JSON_PATH="${DATASETS[$label]}"
    echo "📂 $label: $JSON_PATH"

    max_peak=0

    for i in {1..10}; do
        LOG_FILE="memlog_cujson_${label}_${i}.txt"
        : > "$LOG_FILE"

        log_gpu_memory() {
            while true; do
                nvidia-smi --query-gpu=memory.used --format=csv,noheader,nounits | head -n 1 >> "$LOG_FILE"
                sleep 0.001
            done
        }

        log_gpu_memory &
        logging_pid=$!

        "$BINARY" -b "$JSON_PATH" > /dev/null 2>&1

        kill $logging_pid
        wait $logging_pid 2>/dev/null || true

        peak=$(awk 'BEGIN{max=0} {if($1>max) max=$1} END{print max}' "$LOG_FILE")
        max_peak=$(awk "BEGIN {print ($peak > $max_peak) ? $peak : $max_peak}")
        rm -f "$LOG_FILE"
    done

    echo "$label peak memory: $max_peak MiB"
    echo "$label,$max_peak" >> "$OUT_FILE"
done

echo "✅ cuJSON peak memory results saved to $OUT_FILE"
