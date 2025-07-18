#!/bin/bash -l
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4
#SBATCH --output="result-GPU-cujson-scalability.log"
#SBATCH --mem=8G
#SBATCH -p short_gpu
#SBATCH --gres=gpu:ada6000:1
#SBATCH --time=02:00:00

module load slurm
module load cuda/11.8

# ------------------------------
# Step 1: Paths and Setup
# ------------------------------
mkdir -p results
mkdir -p cujson_results

SRC="../src/reproduced/cuJSON-jsonlines-total-parsing.cu"
BINARY="./cujson_results/output_scalability.exe"
OUT_FILE="results/cujson_fig16.csv"

: > "$OUT_FILE"
echo "Dataset,FileSize,MaxOutputSize(MB)" > "$OUT_FILE"

ORDERED_KEYS=("TT" "BB" "GMD" "NSPL" "WM" "WP")
BASE_DIR="/rhome/aveda002/bigdata/scalability"  # Each folder TT, BB, etc. inside here

# ------------------------------
# Step 2: Compile cuJSON
# ------------------------------
echo "🔧 Compiling cuJSON..."
nvcc -O3 -o "$BINARY" "$SRC" -w -gencode=arch=compute_89,code=sm_89

# ------------------------------
# Step 3: Run each scalability file
# ------------------------------
echo "🚀 Running cuJSON scalability benchmarks..."

for label in "${ORDERED_KEYS[@]}"; do
    DATASET_DIR="$BASE_DIR/$label"

    for JSON_PATH in "$DATASET_DIR"/file_*.json; do
        FILE_SIZE=$(basename "$JSON_PATH" | grep -oP "[0-9]+MB")
        echo "📂 $label - $FILE_SIZE"

        max_size=0
        for i in {1..10}; do
            OUTPUT=$("$BINARY" -b "$JSON_PATH")
            PARSER_MB=$(echo "$OUTPUT" | grep -oP "Parser's Output Size:\s*\K[0-9]+(\.[0-9]+)?")

            INPUT_BYTES=$(stat -c%s "$JSON_PATH")
            INPUT_MB=$(awk "BEGIN {print $INPUT_BYTES / 1024 / 1024}")

            if [[ -z "$PARSER_MB" ]]; then
                echo "❌ Failed to extract parser size for $label/$FILE_SIZE run $i"
                echo "$OUTPUT"
                exit 1
            fi

            TOTAL_MB=$(awk "BEGIN {print $PARSER_MB + $INPUT_MB}")
            max_size=$(awk "BEGIN {print ($TOTAL_MB > $max_size) ? $TOTAL_MB : $max_size}")
        done

        echo "$label,$FILE_SIZE,$max_size" >> "$OUT_FILE"
    done
done

echo "✅ cuJSON scalability output sizes saved to $OUT_FILE"
