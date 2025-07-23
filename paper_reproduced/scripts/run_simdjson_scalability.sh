#!/bin/bash -l

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=8
#SBATCH --output="result-simdjson-scalability.log"
#SBATCH --mem=32G
#SBATCH -p epyc
#SBATCH --time=01:00:00

echo "🔧 Compiling Simdjson scalability benchmark..."
g++ -O3 ../related_works/simdjson/simdjson.cpp ../related_works/simdjson/quickstart-original-iterate-scalability.cpp  -o simdjson_scalability.exe -std=c++17

# ------------------------------
# Set up output
# ------------------------------
mkdir -p results
OUT_FILE="results/simdjson_scalability.csv"
: > "$OUT_FILE"
echo "Size,AverageTime(ms)" > "$OUT_FILE"

# ------------------------------
# Define Sizes
# ------------------------------
SIZES=(2 4 8 16 32 64 128 256)

for SIZE in "${SIZES[@]}"; do
  echo "📏 Running Simdjson benchmark for ${SIZE}MB files..."

  # Run the binary for this size
  # Pass size as env var to filter inside the C++ program
  OUTPUT=$(./simdjson_scalability.exe "$SIZE")

  # Sum all lines and take average manually (C++ already averages per dataset)
  AVG=$(echo "$OUTPUT" | awk -F, 'NR > 1 {sum += $2} END {printf "%.3f", sum / (NR - 1)}')

  echo "${SIZE}MB,${AVG}" >> "$OUT_FILE"
done

echo "✅ All results saved to $OUT_FILE"
