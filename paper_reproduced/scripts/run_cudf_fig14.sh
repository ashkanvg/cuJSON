#!/bin/bash
set -e

echo "🚀 Measuring output memory usage (cuDF, 10× each)..."

cd ../related_works/cuDF/

declare -A SCRIPTS=(
  ["TT"]="twitter.py"
  ["BB"]="bestbuy.py"
  ["GMD"]="google.py"
  ["NSPL"]="nspl.py"
  ["WM"]="walmart.py"
  ["WP"]="wiki.py"
)

mkdir -p ../../scripts/results
OUT_FILE="../../scripts/results/cudf_fig14_parsing_output.csv"
: > "$OUT_FILE"
echo "Dataset,PeakMemory(MiB)" > "$OUT_FILE"

# -------------------------------
# Run each script 10× and extract peak memory
# -------------------------------
for key in TT BB GMD NSPL WM WP; do
  SCRIPT="${SCRIPTS[$key]}"
  echo "▶️  Running $key with $SCRIPT for GPU memory..."

  max_peak=0

  for i in {1..10}; do
    LOG_FILE="gpu_mem_log_${key}_${i}.txt"
    : > "$LOG_FILE"

    # GPU logging function (background)
    log_gpu_memory() {
      while true; do
        echo "$(nvidia-smi --query-gpu=memory.used --format=csv,noheader,nounits | head -n 1)" >> "$LOG_FILE"
        sleep 0.001
      done
    }

    log_gpu_memory &
    logging_pid=$!

    # Run the script (foreground)
    python "$SCRIPT" > /dev/null 2>&1

    # Kill logger
    kill $logging_pid
    wait $logging_pid 2>/dev/null || true

    # Extract peak memory from log
    this_peak=$(awk 'BEGIN{max=0} {if($1>max) max=$1} END{print max}' "$LOG_FILE")
    max_peak=$(awk "BEGIN {print ($this_peak > $max_peak) ? $this_peak : $max_peak}")

    rm -f "$LOG_FILE"
  done

  echo "$key peak GPU memory: $max_peak MiB"
  echo "$key,$max_peak" >> "$OUT_FILE"
done

echo "✅ cuDF parsing memory size results saved to $OUT_FILE"
