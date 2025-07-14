#!/bin/bash
set -e

echo "🚀 Running all Figure 9 benchmarks and generating merged results..."

# Step 1: Run cuJSON
echo "▶️  [1/5] Running cuJSON benchmark..."
bash scripts/run_cujson_fig9.sh

# Step 2: Run simdjson
echo "▶️  [2/5] Running simdjson benchmark..."
bash scripts/run_simdjson_fig9.sh

# Step 3: Run RapidJSON
echo "▶️  [3/5] Running RapidJSON benchmark..."
bash scripts/run_rapidjson_fig9.sh

# Step 4: Run Pison
echo "▶️  [4/5] Running Pison benchmark..."
bash scripts/run_pison_fig9.sh

# Step 5: Merge all results
echo "🧩 [5/5] Merging CSV files..."
bash scripts/merge_fig9_csvs.sh

echo "✅ All done. Final CSV: results/fig9_data.csv"
