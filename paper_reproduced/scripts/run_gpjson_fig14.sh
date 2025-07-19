#!/bin/bash -l
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4
#SBATCH --output="result-GPU-pison-mem.log"
#SBATCH --mem=8G
#SBATCH -p short_gpu
#SBATCH --gres=gpu:ada6000:1
#SBATCH --time=01:00:00

# -------------------------------
# Environment Setup
# -------------------------------
module load slurm
module load cuda/11.8

echo "🔧 Compiling ..."
mkdir -p results

# Compile inside GCC 13 environment
cd ../related_works/pison/output_memory
make clean
make all

# Move into bin after compilation
cd bin

TMP_FILE="../../../../scripts/results/gpjson_fig14.csv"
: > "$TMP_FILE"

echo "Dataset,OutputSize(MB)" > "$TMP_FILE"

ORDERED_KEYS=("TT" "BB" "GMD" "NSPL" "WM" "WP")
declare -A BINARIES=(
    ["TT"]="twitter"
    ["BB"]="bestbuy"
    ["GMD"]="google"
    ["NSPL"]="nspl"
    ["WM"]="walmart"
    ["WP"]="wiki"
)

echo "🚀 Benchmarking GPJSON parsers..."
for key in "${ORDERED_KEYS[@]}"; do
    BIN="${BINARIES[$key]}"
    echo "📂 $key: $BIN"

    SUM=0
    for i in {1..10}; do
        MEM=$(./"$BIN" | grep -Eo '[0-9]+(\.[0-9]+)?' | tail -1)
        SUM=$(awk "BEGIN {print $SUM+$MEM}")
    done
    AVG=$(awk "BEGIN {print $SUM/10}")
    echo "$key,$AVG" >> "$TMP_FILE"
done

echo "✅ GPJSON results saved to $TMP_FILE"
