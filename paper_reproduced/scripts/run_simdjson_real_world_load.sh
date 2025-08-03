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

g++ -O3 ../related_works/simdjson/simdjson.cpp ../related_works/simdjson/quickstart-original-iterate-use-case-query-3.cpp -o simdjson_results/output_large.exe -std=c++17

# -------------------------------------
# Step 2: Run and extract Load Time
# -------------------------------------

echo "🚀 Running simdjson query 20 times..."

TOTAL=0
COUNT=0

for i in {1..20}; do
    echo "Run $i:"
    OUTPUT=$(./simdjson_results/output_large.exe | grep "Load time:")
    echo "$OUTPUT"
    
    # Extract the number from "Load time: xxx ms"
    TIME_MS=$(echo "$OUTPUT" | grep -oP '\d+(\.\d+)?')

    # Skip the first run (warmup)
    if [ $i -ne 1 ]; then
        TOTAL=$(echo "$TOTAL + $TIME_MS" | bc)
        COUNT=$((COUNT + 1))
    fi
done

# Compute average
if [ $COUNT -gt 0 ]; then
    AVG=$(echo "scale=3; $TOTAL / $COUNT" | bc)
    echo "📊 Average Load Time (runs 2-20): $AVG ms"
else
    echo "⚠️ Not enough data to compute average."
fi
