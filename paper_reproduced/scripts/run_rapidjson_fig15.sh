#!/bin/bash

# ------------------------------
# Step 1: Setup
# ------------------------------
echo "🔧 Compiling RapidJSON benchmarks..."
mkdir -p results

g++ -O3 ../related_works/rapidjson/main-query.cpp -o ./output-query.exe
# ------------------------------
# Step 2: Run each binary 
# ------------------------------

./output-query.exe > "results/rapidjson_fig15.csv"

echo "✅ RapidJSON results saved to $TMP_FILE"
