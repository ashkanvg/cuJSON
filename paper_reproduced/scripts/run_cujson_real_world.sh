#!/bin/bash -l

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=8
#SBATCH --output="result-GPU-realworld-usecase.log"
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

SRC="../src/reproduced/cuJSON-standardjson-total-parsing-2.cu"
# SRC="../../real_world_use_case/main.cu"
BINARY="./cujson_results/output_large.exe"

# BASE_DIR="../../dataset/merged_output.json"
BASE_DIR="../../dataset/github_archive_large_record.json"
# BASE_DIR="../../dataset/github_archive_small_records.json"
# BASE_DIR="../../dataset/merged_output_large.json"

# ------------------------------
# Step 2: Compile
# ------------------------------
echo "🔧 Compiling cuJSON..."
nvcc -O3 "$SRC" -o "$BINARY" -w -gencode=arch=compute_80,code=sm_80
# nvcc -O3 "$SRC" -o "$BINARY" -w -gencode=arch=compute_80,code=sm_80


# ------------------------------
# Step 3: Run and collect results
# ------------------------------
echo "🚀 Benchmarking r (cuJSON parsing time)..."

# "$BINARY" "$BASE_DIR" 
"$BINARY" -b "$BASE_DIR" 