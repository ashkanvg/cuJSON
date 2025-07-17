#!/bin/bash
set -e

echo "🚀 Running all Figure 11 benchmarks and generating merged results..."

# Step 1: Run cuJSON
echo "▶️  [1/4] Running cuJSON benchmark..."
bash scripts/run_cujson_fig9.sh

# Step 2: Run simdjson
echo "▶️  [2/4] Running cudf benchmark..."
bash scripts/run_cudf_fig11.sh

# Step 3: Run RapidJSON
echo "▶️  [3/4] Running gpjson benchmark..."
bash scripts/run_gpjson_fig11.sh

# Step 5: Merge all results
echo "🧩 [4/4] Merging CSV files..."
bash scripts/merge_fig11_csvs.sh

echo "✅ All done. Final CSV: results/fig11_data.csv"
