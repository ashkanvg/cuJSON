#!/bin/bash
set -e

# ------------------------------
# Step 1: Input CSV paths
# ------------------------------
CUJSON="results/cujson_fig15.csv"
simdjson="results/simdjson_fig15.csv"
pison="results/pison_fig15.csv"
rapidjson="results/rapidjson_fig15.csv"
MERGED="results/fig15_data.csv"

# ------------------------------
# Step 2: Output header
# ------------------------------
echo "Method,cuJSON,simdjson,pison,rapidjson" > "$MERGED"

# ------------------------------
# Step 3: Extract and merge AVERAGE values line-by-line
# ------------------------------
# Extract the "AVERAGE" values for each method from the CSVs and merge them
cu_val=$(grep "^AVERAGE," "$CUJSON" | cut -d',' -f2)
simdjson_val=$(grep "^AVERAGE," "$simdjson" | cut -d',' -f2)
pison_val=$(grep "^AVERAGE," "$pison" | cut -d',' -f2)
rapidjson_val=$(grep "^AVERAGE," "$rapidjson" | cut -d',' -f2)

# Write the results for the "AVERAGE" key
echo "Time (ns),$cu_val,$simdjson_val,$pison_val,$rapidjson_val" >> "$MERGED"

echo "✅ Combined results saved to $MERGED"
