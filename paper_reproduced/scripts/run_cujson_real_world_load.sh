#!/bin/bash -l

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=8
#SBATCH --output="result-GPU-realworld-usecase-load.log"
#SBATCH --mem=16G
#SBATCH -p gpu
#SBATCH --gres=gpu:a100:1
#SBATCH --time=00:10:00

# Load needed modules
module load slurm
module load cuda/11.8

# ------------------------------
# Step 1: Setup
# ------------------------------
mkdir -p results
mkdir -p cujson_results

SRC="../../real_world_use_case/main_load.cu"
BINARY="./cujson_results/output_large.exe"
BASE_DIR="../../dataset/github_archive_small_records.json"

# ------------------------------
# Step 2: Compile
# ------------------------------
echo "🔧 Compiling cuJSON..."
nvcc -O3 "$SRC" -o "$BINARY" -w -gencode=arch=compute_80,code=sm_80

# ------------------------------
# Step 3: Run and collect results
# ------------------------------
echo "🚀 Benchmarking cuJSON parsing time (10 repetitions)..."
for i in {1..10}; do
    echo "▶️ Run #$i"
    "$BINARY" "$BASE_DIR"
done
