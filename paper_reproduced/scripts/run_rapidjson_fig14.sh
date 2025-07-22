#!/bin/bash

# ------------------------------
# Step 1: Setup
# ------------------------------
echo "🔧 Compiling RapidJSON benchmarks..."
mkdir -p results

cd ../related_works/rapidjson/  # Adjust if needed

OUT_FILE="../../scripts/results/rapidjson_fig14.csv"
: > "$OUT_FILE"
echo "Dataset,MaxMemoryUsage(MB)" > "$OUT_FILE"

ORDERED_KEYS=("TT" "BB" "GMD" "NSPL" "WM" "WP")

declare -A SOURCES=(
    ["TT"]="./main-twitter-memory.cpp"
    ["BB"]="./main-bestbuy-memory.cpp"
    ["GMD"]="./main-google-memory.cpp"
    ["NSPL"]="./main-nspl-memory.cpp"
    ["WM"]="./main-walmart-memory.cpp"
    ["WP"]="./main-wiki-memory.cpp"
)

# ------------------------------
# Step 2: Compile each source file
# ------------------------------
echo "🛠️  Compiling source files..."
for key in "${ORDERED_KEYS[@]}"; do
    SRC="${SOURCES[$key]}"
    OUT_EXE="output-${key,,}.exe"  # e.g., output-tt.exe
    echo "📦 Compiling $key → $OUT_EXE"
    g++ -O3 "$SRC" -o "$OUT_EXE"
done

# ------------------------------
# Step 3: Run each binary 10× to extract memory usage
# ------------------------------
echo "🚀 Measuring RapidJSON Output Memory Usage..."
for key in "${ORDERED_KEYS[@]}"; do
    BIN="output-${key,,}.exe"
    echo "▶️  $key: $BIN"

    max_mem=0
    for i in {1..10}; do
        OUTPUT=$(./"$BIN")
        MEM_MB=$(echo "$OUTPUT" | grep -oP "Memory Usage:\s*\K[0-9]+(\.[0-9]+)?")

        if [[ -z "$MEM_MB" ]]; then
            echo "❌ Failed to extract memory for $key (run $i)"
            echo "$OUTPUT"
            exit 1
        fi

        max_mem=$(awk "BEGIN {print ($MEM_MB > $max_mem) ? $MEM_MB : $max_mem}")
    done

    echo "$key,$max_mem" >> "$OUT_FILE"
    echo "✅ $key max memory usage: $max_mem MB"
done

echo "✅ RapidJSON memory usage results saved to $OUT_FILE"
