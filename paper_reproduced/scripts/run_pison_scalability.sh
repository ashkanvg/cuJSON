#!/bin/bash -l

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=8
#SBATCH --output="result-pison-scalability.log"
#SBATCH --mem=32G
#SBATCH -p epyc
#SBATCH --time=01:00:00

# -------------------------------
# Environment Setup
# -------------------------------
echo "🔧 Compiling Pison (with gcc-toolset)..."
mkdir -p results

# Compile inside GCC 13 environment
cd ../related_works/pison/scalability && make clean && make all

cd bin

# -------------------------------
# Output CSV
# -------------------------------
OUT_FILE="../../../../scripts/results/pison_scalability.csv"
: > "$OUT_FILE"
echo "Size,AverageTime(ms)" > "$OUT_FILE"

# -------------------------------
# Benchmark Configuration
# -------------------------------
ORDERED_KEYS=("TT" "BB" "GMD" "NSPL" "WM" "WP")
declare -A BINARIES=(
    ["TT"]="twitter"
    ["BB"]="bestbuy"
    ["GMD"]="google"
    ["NSPL"]="nspl"
    ["WM"]="walmart"
    ["WP"]="wiki"
)

SIZES=(2 4 8 16 32 64 128 256)

# -------------------------------
# Run all benchmarks
# -------------------------------
for SIZE in "${SIZES[@]}"; do
  echo "📏 Running Pison for ${SIZE}MB files..."

  SUM=0
  COUNT=0

  for key in "${ORDERED_KEYS[@]}"; do
    BIN="${BINARIES[$key]}"
    echo "  ▶️  Dataset: $key"

    TOTAL=0
    for i in {1..10}; do
      RAW=$(./"$BIN" "$SIZE" | tail -n 1)
      TIME=$(awk "BEGIN {print $RAW}")  # validate it's a number
      TOTAL=$(awk "BEGIN {print $TOTAL + $TIME}")
    done

    AVG_DATASET=$(awk "BEGIN {print $TOTAL / 10}")
    SUM=$(awk "BEGIN {print $SUM + $AVG_DATASET}")
    COUNT=$((COUNT + 1))
  done

  AVG_TOTAL=$(awk "BEGIN {print $SUM / $COUNT}")
  echo "${SIZE}MB,${AVG_TOTAL}" >> "$OUT_FILE"
done

echo "✅ Pison scalability results saved to $OUT_FILE"
