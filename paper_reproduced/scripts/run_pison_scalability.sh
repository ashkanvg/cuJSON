#!/bin/bash -l

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=64
#SBATCH --output="result-pison-thread-vs-size.log"
#SBATCH --mem=64G
#SBATCH -p epyc
#SBATCH --time=01:00:00

# -------------------------------------
# Compilation
# -------------------------------------
echo "🔧 Compiling Pison benchmarks..."
cd ../related_works/pison/scalability && make clean && make all
cd bin

# -------------------------------------
# Configurations
# -------------------------------------
THREADS=(1 2 4 8 16 32 64)
SIZES=(2 4 8 16 32 64 128 256)

ORDERED_KEYS=("TT" "BB" "GMD" "NSPL" "WM" "WP")
declare -A BINARIES=(
    ["TT"]="twitter"
    ["BB"]="bestbuy"
    ["GMD"]="google"
    ["NSPL"]="nspl"
    ["WM"]="walmart"
    ["WP"]="wiki"
)

OUT_DIR="../../../../scripts/results/pison_thread_vs_size"
mkdir -p "$OUT_DIR"

# -------------------------------------
# Run benchmarks and write CSVs
# -------------------------------------
for key in "${ORDERED_KEYS[@]}"; do
  BIN="${BINARIES[$key]}"
  OUT_FILE="${OUT_DIR}/${key}_thread_vs_size.csv"

  echo "📊 Benchmarking ${key} (${BIN})..."
  : > "$OUT_FILE"

  # Write header row
  HEADER="Threads"
  for SIZE in "${SIZES[@]}"; do
    HEADER="$HEADER,${SIZE}MB"
  done
  echo "$HEADER" >> "$OUT_FILE"

  # Benchmark loop
  for THREAD in "${THREADS[@]}"; do
    echo "  🧵 Running ${key} with ${THREAD} threads..."
    ROW="$THREAD"

    for SIZE in "${SIZES[@]}"; do
      SUM=0
      for i in {1..5}; do
        RAW=$(./"$BIN" "$SIZE" "$THREAD" | tail -n 1)
        TIME=$(awk "BEGIN {print $RAW}")
        SUM=$(awk "BEGIN {print $SUM + $TIME}")
      done
      AVG=$(awk "BEGIN {print $SUM / 5.0}")
      ROW="$ROW,$AVG"
    done

    echo "$ROW" >> "$OUT_FILE"
  done

  # -------------------------------------
  # Append minimums row
  # -------------------------------------
  echo "📉 Computing min row for $key..."

  MIN_ROW="Min"
  for COL in $(seq 2 $((${#SIZES[@]} + 1))); do
    MIN_VAL=$(awk -F',' -v col=$COL 'NR==2 {min=$col} NR>2 {if ($col < min) min=$col} END {print min}' "$OUT_FILE")
    MIN_ROW="$MIN_ROW,$MIN_VAL"
  done

  echo "$MIN_ROW" >> "$OUT_FILE"
  echo "✅ Saved to $OUT_FILE"
done

echo "🎉 All Pison thread-vs-size benchmarks completed!"
