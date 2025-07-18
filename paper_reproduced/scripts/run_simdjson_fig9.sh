#!/bin/bash
set -e

# -------------------------------------
# Step 1: Compile
# -------------------------------------
echo "🔧 Compiling simdjson..."
mkdir -p simdjson_results
mkdir -p results

g++ -O3 ../related_works/simdjson/simdjson.cpp ../related_works/simdjson/quickstart-original-iterate.cpp -o simdjson_results/output_large.exe -std=c++17

# -------------------------------------
# Step 2: Run and save output
# -------------------------------------

OUT_FILE="results/simdjson_fig9.csv"

echo "🚀 Running simdjson results:..."
./simdjson_results/output_large.exe > "$OUT_FILE"

# -------------------------------------
# Step 3: Merge with cuJSON results
# -------------------------------------
# Create merged CSV (Dataset,cuJSON,simdjson)
# TMP_CSV="results/tmp_fig9_combined.csv"

# Read cuJSON and simdjson line by line and join
# paste -d',' <(tail -n +2 "$FINAL_CSV") <(cut -d',' -f2 "$OUT_FILE") > "$TMP_CSV"

# Re-add header
# echo "Dataset,cuJSON,simdjson" > "$FINAL_CSV"
# cat "$TMP_CSV" >> "$FINAL_CSV"

# echo "✅ Combined cuJSON + simdjson results saved to $FINAL_CSV"
