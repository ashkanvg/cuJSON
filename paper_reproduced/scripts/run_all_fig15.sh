#!/bin/bash


echo "🚀 Running all Figure 12 benchmarks and generating merged results..."

# Step 1: Run cuJSON
echo "▶️  [1/4] Running cuJSON benchmark..."
bash ./run_cujson_fig15.sh

# Step 2: Run simdjson
echo "▶️  [2/4] Running simdjson benchmark..."
bash ./run_simdjson_fig15.sh

# Step 3: Run RapidJSON
echo "▶️  [3/4] Running RapidJSON benchmark..."
bash ./run_rapidjson_fig15.sh

# Step 3: Run RapidJSON
echo "▶️  [3/4] Running pison benchmark..."
bash ./run_pison_fig15.sh

# Step 4: Merge all results
echo "🧩 [4/4] Merging CSV files..."
bash ./merge_fig15_csvs.sh

echo "✅ All done. Final CSV: results/fig15_data.csv"
