#!/bin/bash -l
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4
#SBATCH --output="result-GPU-pison-query.log"
#SBATCH --mem=8G
#SBATCH -p short_gpu
#SBATCH --gres=gpu:ada6000:1
#SBATCH --time=01:00:00

# -------------------------------
# Environment Setup
# -------------------------------
module load slurm
module load cuda/11.8

echo "🔧 Compiling Pison (with gcc-toolset)..."
mkdir -p results

# Compile inside GCC 13 environment
cd ../related_works/pison/query && make clean && make all

# Move into bin after compilation
cd bin

TMP_FILE="../../../../scripts/results/pison_fig15_tmp.csv"
: > "$TMP_FILE"

ORDERED_KEYS=("TT" "BB" "GMD" "NSPL" "WM" "WP")
declare -A BINARIES=(
    ["TT"]="twitter"
    ["BB"]="bestbuy"
    ["GMD"]="google"
    ["NSPL"]="nspl"
    ["WM"]="walmart"
    ["WP"]="wiki"
)

echo "🚀 Benchmarking Pison parsers..."
for key in "${ORDERED_KEYS[@]}"; do
    BIN="${BINARIES[$key]}"
    echo "📂 $key: $BIN"

    SUM=0
    for i in {1..10}; do
        TIME=$(./"$BIN" | tail -n 1)
        SUM=$(awk "BEGIN {print $SUM+$TIME}")
    done
    AVG=$(awk "BEGIN {print $SUM/10}")
    echo "$key,$AVG" >> "$TMP_FILE"
done

echo "✅ Pison results saved to $TMP_FILE"
