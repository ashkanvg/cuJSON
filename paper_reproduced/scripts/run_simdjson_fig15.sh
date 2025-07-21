#!/bin/bash
set -e

# -------------------------------------
# Step 1: Compile
# -------------------------------------
echo "🔧 Compiling simdjson..."
mkdir -p simdjson_results
mkdir -p results

g++ -O3 ../related_works/simdjson/simdjson.cpp ../related_works/simdjson/quickstart-original-iterate-query.cpp -o simdjson_results/output_large.exe -std=c++17

# -------------------------------------
# Step 2: Run and save output
# -------------------------------------

OUT_FILE="results/simdjson_fig15.csv"

echo "🚀 Running simdjson query results:..."
# echo "Dataset,TotalAvgQuery" > "$OUT_FILE"
./simdjson_results/output_large.exe > "$OUT_FILE"

