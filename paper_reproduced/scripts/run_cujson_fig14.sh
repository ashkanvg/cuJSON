#!/bin/bash -l
#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4
#SBATCH --output="result-GPU-cujson-outputsize.log"
#SBATCH --mem=8G
#SBATCH -p short_gpu
#SBATCH --gres=gpu:ada6000:1
#SBATCH --time=01:00:00

module load slurm
module load cuda/11.8

# ------------------------------
# Step 1: Paths and Setup
# ------------------------------
mkdir -p results
mkdir -p cujson_results

SRC="../src/reproduced/cuJSON-standardjson-parser-output.cu"
BINARY="./cujson_results/output_large.exe"
OUT_FILE="results/cujson_fig14_parsing_output.csv"

: > "$OUT_FILE"
echo "Dataset,MaxOutputSize(MB)" > "$OUT_FILE"

ORDERED_KEYS=("TT" "BB" "GMD" "NSPL" "WM" "WP")

declare -A DATASETS=(
    ["TT"]="/rhome/aveda002/bigdata/Test-Files/twitter_small_records_remove.json"
    ["BB"]="/rhome/aveda002/bigdata/Test-Files/bestbuy_small_records_remove.json"
    ["GMD"]="/rhome/aveda002/bigdata/Test-Files/google_map_small_records_remove.json"
    ["NSPL"]="/rhome/aveda002/bigdata/Test-Files/nspl_small_records_remove.json"
    ["WM"]="/rhome/aveda002/bigdata/Test-Files/walmart_small_records_remove.json"
    ["WP"]="/rhome/aveda002/bigdata/Test-Files/wiki_small_records_remove.json"
)

# ------------------------------
# Step 2: Compile cuJSON
# ------------------------------
echo "🔧 Compiling cuJSON..."
nvcc -O3 -o "$BINARY" "$SRC" -w -gencode=arch=compute_89,code=sm_89

# ------------------------------
# Step 3: Run and Extract Output Size
# ------------------------------
echo "🚀 Measuring cuJSON Output Size..."

for label in "${ORDERED_KEYS[@]}"; do
    JSON_PATH="${DATASETS[$label]}"
    echo "📂 $label: $JSON_PATH"

    max_size=0
    for i in {1..10}; do
        OUTPUT=$("$BINARY" -b "$JSON_PATH")

        # Extract output size from cuJSON stdout
        PARSER_MB=$(echo "$OUTPUT" | grep -oP "Parser's Output Size:\s*\K[0-9]+(\.[0-9]+)?")

        # Compute input .json file size in MB
        INPUT_BYTES=$(stat -c%s "$JSON_PATH")
        INPUT_MB=$(awk "BEGIN {print $INPUT_BYTES / 1024 / 1024}")

        # Validate extraction
        if [[ -z "$PARSER_MB" ]]; then
            echo "❌ Failed to extract parser size for $label run $i"
            echo "$OUTPUT"
            exit 1
        fi

        # Compute total output size
        TOTAL_MB=$(awk "BEGIN {print $PARSER_MB + $INPUT_MB}")
        echo "Run $i total output size: $TOTAL_MB MB"

        # Track max over 10 runs
        max_size=$(awk "BEGIN {print ($TOTAL_MB > $max_size) ? $TOTAL_MB : $max_size}")
    done

    echo "$label,$max_size" >> "$OUT_FILE"
    echo "✅ $label max output size: $max_size MB"
done

echo "✅ cuJSON output sizes saved to $OUT_FILE"
