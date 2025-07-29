#!/bin/bash
# ------------------------------
# Step 1: Setup
# ------------------------------
echo "🔧 Compiling RapidJSON benchmarks..."
mkdir -p results

cd ../related_works/rapidjson/  # Path to .cpp and .exe files

ORDERED_KEYS=("RW")
# ORDERED_KEYS=("TT" "BB" "GMD" "NSPL" "WM" "WP")

declare -A SOURCES=(
    ["RW"]="main-real-world-query.cpp"
)

declare -A BINARIES=(
    ["RW"]="output-real-world.exe"
)

# ------------------------------
# Step 2: Compile each binary
# ------------------------------
for key in "${ORDERED_KEYS[@]}"; do
    SRC="${SOURCES[$key]}"
    BIN="${BINARIES[$key]}"
    echo "🔨 Compiling $SRC -> $BIN"
    g++ -O3 "$SRC" -o "$BIN" -std=c++17
done

# ------------------------------
# Step 3: Run each binary 10 times and print all output
# ------------------------------
echo "🚀 Benchmarking RapidJSON parsers..."
for key in "${ORDERED_KEYS[@]}"; do
    BIN="${BINARIES[$key]}"
    echo "📂 $key: $BIN"

    SUM=0
    for i in {1..10}; do
        echo "🔁 Run #$i:"
        OUTPUT=$(./"$BIN")
        echo "$OUTPUT"

        # Extract the last number in the output (assumes it's total time)
        TIME=$(echo "$OUTPUT" | grep -Eo '[0-9]+(\.[0-9]+)?' | tail -1)
        SUM=$(awk "BEGIN {print $SUM + $TIME}")
    done

    AVG=$(awk "BEGIN {print $SUM / 10}")
done

echo "✅ RapidJSON results saved to $TMP_FILE"
