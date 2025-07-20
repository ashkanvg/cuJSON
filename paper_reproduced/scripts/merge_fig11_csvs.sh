#!/bin/bash
set -e

# ------------------------------
# Step 1: Input CSV paths
# ------------------------------
CUJSON="results/cujson_fig9.csv"
cudf="results/cudf_fig11.csv"
gpjson="results/gpjson_fig11.csv"
MERGED="results/fig11_data.csv"

# Ordered list of datasets (enforces row order)
ORDERED_KEYS=("TT" "BB" "GMD" "NSPL" "WM" "WP")

# ------------------------------
# Step 2: Output header
# ------------------------------
echo "Dataset,cuJSON,cuDF,GPJSON" > "$MERGED"

# ------------------------------
# Step 3: Merge values line-by-line
# ------------------------------
for dataset in "${ORDERED_KEYS[@]}"; do
    cu_val=$(grep "^$dataset," "$CUJSON" | cut -d',' -f2)
    cudf_val=$(grep "^$dataset," "$cudf" | cut -d',' -f2)
    gpjson_val=$(grep "^$dataset," "$gpjson" | cut -d',' -f2)

    echo "$dataset,$cu_val,$cudf_val,$gpjson_val" >> "$MERGED"
done

echo "✅ Combined results saved to $MERGED"
