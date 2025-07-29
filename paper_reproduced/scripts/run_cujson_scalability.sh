#!/bin/bash -l

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --output="result-cujson-scalability-a100.log"
#SBATCH --mem=8G
#SBATCH --partition gpu
#SBATCH --gres=gpu:a100:1
#SBATCH --time=02:00:00

# Load needed modules
module load slurm
module load cuda/11.8


# ------------------------------
# Step 1: Paths and Setup
# ------------------------------
mkdir -p results/cujson_scalability
mkdir -p cujson_results

SRC="../src/reproduced/cuJSON-standardjson-total-parsing.cu"
BINARY="./cujson_results/output_large.exe"

# Dataset keys and size range
ORDERED_KEYS=("TT" "BB" "GMD" "NSPL" "WM" "WP")
SIZES=(2 4 8 16 32 64 128 256)

# Map keys to file path template
declare -A DATASET_PATHS=(
    ["TT"]="/rhome/aveda002/bigdata/Test-Files/scalability/tt/output_%dMB_large.json"
    ["BB"]="/rhome/aveda002/bigdata/Test-Files/scalability/bb/output_%dMB_large.json"
    ["GMD"]="/rhome/aveda002/bigdata/Test-Files/scalability/gmp/output_%dMB_large.json"
    ["NSPL"]="/rhome/aveda002/bigdata/Test-Files/scalability/nspl/output_%dMB_large.json"
    ["WM"]="/rhome/aveda002/bigdata/Test-Files/scalability/wm/output_%dMB_large.json"
    ["WP"]="/rhome/aveda002/bigdata/Test-Files/scalability/wp/output_%dMB_large.json"
)

# ------------------------------
# Step 2: Compile cuJSON
# ------------------------------
echo "🔧 Compiling cuJSON..."
nvcc -O3 -o "$BINARY" "$SRC" -w -gencode=arch=compute_80,code=sm_80

# ------------------------------
# Step 3: Run Benchmark
# ------------------------------
for key in "${ORDERED_KEYS[@]}"; do
    OUT_FILE="results/cujson_scalability/${key}_cujson_scalability.csv"
    echo "📊 Benchmarking $key..."
    echo "SizeMB,AvgTime(ms)" > "$OUT_FILE"

    for SIZE in "${SIZES[@]}"; do
        FILE_PATH=$(printf "${DATASET_PATHS[$key]}" "$SIZE")
        echo "  ▶️ ${SIZE}MB - $FILE_PATH"

        TOTAL=0
        for i in {1..5}; do
            TIME=$("$BINARY" -b "$FILE_PATH" | grep -Eo '[0-9]+(\.[0-9]+)?' | tail -1)
            TOTAL=$(awk "BEGIN {print $TOTAL + $TIME}")
        done
        AVG=$(awk "BEGIN {print $TOTAL / 5.0}")
        echo "${SIZE},${AVG}" >> "$OUT_FILE"
    done

    # ------------------------------
    # Step 4: Append Minimum Time Row
    # ------------------------------
    MIN_VAL=$(awk -F',' 'NR==2 {min=$2} NR>2 {if ($2 < min) min=$2} END {print min}' "$OUT_FILE")
    echo "Min,${MIN_VAL}" >> "$OUT_FILE"
    echo "✅ Saved $OUT_FILE"
done

echo "🎉 cuJSON scalability benchmarking complete!"


# ------------------------------
# Step 5: Compute Per-Size Average Across Datasets
# ------------------------------
echo "📊 Computing per-size average across all datasets..."

SUMMARY_FILE="results/cujson_scalability/cujson_scalability_summary.csv"
echo "SizeMB,AvgAcrossDatasets(ms)" > "$SUMMARY_FILE"

for SIZE in "${SIZES[@]}"; do
  SUM=0
  COUNT=0
  for key in "${ORDERED_KEYS[@]}"; do
    FILE="results/cujson_scalability/${key}_cujson_scalability.csv"
    VALUE=$(awk -F',' -v s="$SIZE" '$1==s {print $2}' "$FILE")
    if [[ -n "$VALUE" ]]; then
      SUM=$(awk "BEGIN {print $SUM + $VALUE}")
      COUNT=$((COUNT + 1))
    fi
  done
  AVG=$(awk "BEGIN {print $SUM / $COUNT}")
  echo "${SIZE},${AVG}" >> "$SUMMARY_FILE"
done

echo "✅ Final average summary saved to $SUMMARY_FILE"
