#!/bin/bash 

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=64
#SBATCH --output="result-simdjson-real-world.log"
#SBATCH --mem=32G
#SBATCH -p epyc
#SBATCH --time=01:00:00

# -------------------------------------
# Step 1: Compile
# -------------------------------------
echo "🔧 Compiling simdjson..."
mkdir -p simdjson_results
mkdir -p results

g++ -O3 ../related_works/simdjson/simdjson.cpp ../related_works/simdjson/quickstart-original-iterate-use-case-query-2.cpp -o simdjson_results/output_large.exe -std=c++17
# g++ -O3 ../related_works/simdjson/simdjson.cpp ../related_works/simdjson/quickstart-original-iterate-use-case-query.cpp -o simdjson_results/output_large.exe -std=c++17

# -------------------------------------
# Step 2: Run and save output
# -------------------------------------

# OUT_FILE="results/simdjson_fig15.csv"

echo "🚀 Running simdjson query results:..."
# echo "Dataset,TotalAvgQuery" > "$OUT_FILE"
./simdjson_results/output_large.exe

