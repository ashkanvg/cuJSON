#!/bin/bash
set -e

# ------------------------------
# Step 1: Input CSV paths
# ------------------------------
CUJSON="results/cujson_fig9.csv"
SIMDJSON="results/simdjson_fig9.csv"
RAPIDJSON="results/rapidjson_fig9.csv"
PISON="results/pison_fig9.csv"
MERGED="results/fig9_data.csv"

# Ordered list of datasets (enforces row order)
ORDERED_KEYS=("TT" "BB" "GMD" "NSPL" "WM" "WP")

# ------------------------------
# Step 2: Output header
# ------------------------------
echo "Dataset,cuJSON,simdjson,RapidJSON,Pison" > "$MERGED"

# ------------------------------
# Step 3: Merge values line-by-line
# ------------------------------
for dataset in "${ORDERED_KEYS[@]}"; do
    cu_val=$(grep "^$dataset," "$CUJSON" | cut -d',' -f2)
    simd_val=$(grep "^$dataset," "$SIMDJSON" | cut -d',' -f2)
    rapid_val=$(grep "^$dataset," "$RAPIDJSON" | cut -d',' -f2)
    pison_val=$(grep "^$dataset," "$PISON" | cut -d',' -f2)

    echo "$dataset,$cu_val,$simd_val,$rapid_val,$pison_val" >> "$MERGED"
done

echo "✅ Combined results saved to $MERGED"
