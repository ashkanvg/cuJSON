#!/bin/bash
set -e

export GRAALVM_HOME=/rhome/aveda002/bigdata/gpjson/graalvm-ce-java8-21.0.0.2
export JAVA_HOME=$GRAALVM_HOME
export PATH=$GRAALVM_HOME/bin:$PATH

echo "🚀 Running GPJSON peak GPU memory benchmark (10× per dataset)..."

# -------------------------------
# Dataset-to-Script Mapping
# -------------------------------
declare -A SCRIPTS=(
  ["TT"]="../related_works/gpjson/gpjson-twitter.js"
  ["BB"]="../related_works/gpjson/gpjson-bestbuy.js"
  ["GMD"]="../related_works/gpjson/gpjson-google.js"
  ["NSPL"]="../related_works/gpjson/gpjson-nspl.js"
  ["WM"]="../related_works/gpjson/gpjson-walmart.js"
  ["WP"]="../related_works/gpjson/gpjson-wiki.js"
)



# -------------------------------
# Output File Setup
# -------------------------------
mkdir -p results
OUT_FILE="results/gpjson_fig12.csv"
: > "$OUT_FILE"
echo "Dataset,PeakMemory(MiB)" > "$OUT_FILE"

# -------------------------------
# Run Each Dataset Script 10× and Record Peak Memory
# -------------------------------
for key in TT BB GMD NSPL WM WP; do
  SCRIPT="${SCRIPTS[$key]}"
  echo "▶️  Measuring peak GPU memory for $key ($SCRIPT)"

  max_peak=0

  for i in {1..10}; do
    LOG_FILE="gpu_mem_log_${key}_${i}.txt"
    : > "$LOG_FILE"

    # Launch GPU memory logger
    log_gpu_memory() {
      while true; do
        nvidia-smi --query-gpu=memory.used --format=csv,noheader,nounits | head -n 1 >> "$LOG_FILE"
        sleep 0.001
      done
    }

    log_gpu_memory &
    logging_pid=$!

    # Run GPJSON script
    $GRAALVM_HOME/bin/node --polyglot --jvm "$SCRIPT" > /dev/null 2>&1

    kill $logging_pid
    wait $logging_pid 2>/dev/null || true

    # Compute peak memory
    this_peak=$(awk 'BEGIN{max=0} {if($1>max) max=$1} END{print max}' "$LOG_FILE")
    max_peak=$(awk "BEGIN {print ($this_peak > $max_peak) ? $this_peak : $max_peak}")
    rm -f "$LOG_FILE"
  done

  echo "$key peak memory: $max_peak MiB"
  echo "$key,$max_peak" >> "$OUT_FILE"
done

echo "✅ Peak GPU memory saved to $OUT_FILE"
