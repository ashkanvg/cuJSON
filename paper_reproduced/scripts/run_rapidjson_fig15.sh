#!/bin/bash

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=8
#SBATCH --output="result-rapidjson-query.log"
#SBATCH --mem=32G
#SBATCH -p epyc # This is the default partition, you can use any of the following; intel, batch, highmem, gpu

# ------------------------------
# Step 1: Setup
# ------------------------------
echo "🔧 Compiling RapidJSON benchmarks..."
mkdir -p results

g++ -O3 ../related_works/rapidjson/main-query.cpp -o "output-query.exe"
# ------------------------------
# Step 2: Run each binary 
# ------------------------------

./output-query.exe > "results/rapidjson_fig15_tmp.csv"

echo "✅ RapidJSON results saved to $TMP_FILE"
