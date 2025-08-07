#!/bin/bash -l

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=8
#SBATCH --output="result-cudf-realworld-usecase.log"
#SBATCH --mem=16G
#SBATCH -p short_gpu
#SBATCH --gres=gpu:ada6000:1
#SBATCH --time=00:10:00

# Load needed modules
module load slurm
module load cuda/11.8


echo "🚀 Running cuDF benchmarks (10× each) and computing averages..."
python -c "import cudf; print(cudf.__version__)"

cat /opt/conda/meta/pinned

# # -------------------------------
# # cuDF Benchmark Scripts (Python)
# # -------------------------------
# cd ../related_works/cuDF/

# declare -A SCRIPTS=(
#   ["TT"]="twitter.py"
#   ["BB"]="bestbuy.py"
#   ["GMD"]="google.py"
#   ["NSPL"]="nspl.py"
#   ["WM"]="walmart.py"
#   ["WP"]="wiki.py"
# )

# mkdir -p ../../scripts/results
# OUT_FILE="../../scripts/results/cudf_fig11.csv"
# : > "$OUT_FILE"
# echo "Dataset,AverageTime(ms)" > "$OUT_FILE"

# # -------------------------------
# # Run each script 10× and extract timing
# # -------------------------------
# for key in TT BB GMD NSPL WM WP; do
#   SCRIPT="${SCRIPTS[$key]}"
#   echo "▶️  Running $key with $SCRIPT"

#   sum=0
#   for i in {1..10}; do
#     TIME_MS=$(python "$SCRIPT" 2>&1 | grep "Time taken to parse:" | grep -Eo '[0-9]+\.[0-9]+')
#     sum=$(awk "BEGIN {print $sum + $TIME_MS}")
#   done

#   avg=$(awk "BEGIN {print $sum / 10}")
#   echo "$key average: $avg ms"
#   echo "$key,$avg" >> "$OUT_FILE"
# done

# echo "✅ cuDF results saved to $OUT_FILE"
