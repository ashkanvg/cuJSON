#!/bin/bash -l

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=64
#SBATCH --output="result-pison-real-world.log"
#SBATCH --mem=32G
#SBATCH -p epyc
#SBATCH --time=01:00:00

echo "🔧 Compiling Pison..."
mkdir -p results

cd ../related_works/pison/real_world_2
make clean 
make all

cd ./bin

ORDERED_KEYS=("RW")
declare -A BINARIES=(
    ["RW"]="real_world"
)

THREADS_LIST=(1 2 4 8 16)
# THREADS_LIST=(1 2 4 8 16 32 64)

echo "🚀 Finding best thread count based on parsing time..."
BEST_THREAD=-1
BEST_PARSE_TIME=9999999

for thread in "${THREADS_LIST[@]}"; do
    echo "🔄 Testing thread count: $thread"
    TOTAL_PARSE=0

    for i in {1..3}; do
        OUTPUT=$(./real_world "$thread")
        PARSE_TIME=$(echo "$OUTPUT" | grep "Parse time" | grep -Eo '[0-9]+\.[0-9]+')
        echo "Run $i - Parse time: $PARSE_TIME ms"
        TOTAL_PARSE=$(awk "BEGIN {print $TOTAL_PARSE + $PARSE_TIME}")
    done

    AVG_PARSE=$(awk "BEGIN {print $TOTAL_PARSE / 3}")
    echo "Thread $thread - Avg parse time: $AVG_PARSE ms"

    # Check if this is the best
    COMP=$(awk "BEGIN {print ($AVG_PARSE < $BEST_PARSE_TIME) ? 1 : 0}")
    if [ "$COMP" -eq 1 ]; then
        BEST_THREAD=$thread
        BEST_PARSE_TIME=$AVG_PARSE
    fi
done

echo "✅ Best thread count: $BEST_THREAD with avg parse time: $BEST_PARSE_TIME ms"
echo ""

# -----------------------------
# Final Benchmarking 10×
# -----------------------------
echo "🚀 Running final benchmark 10× with $BEST_THREAD threads..."

SUM_PARSE=0
SUM_QUERY=0
SUM_TOTAL=0

ALL_OUTPUTS=""  # Collect all output here

for i in {1..10}; do
    echo "🔁 Run $i:"
    OUTPUT=$(./real_world "$BEST_THREAD")
    echo "$OUTPUT"
    
    # Append to full output
    ALL_OUTPUTS+="🔁 Run $i:\n$OUTPUT\n\n"

    PARSE=$(echo "$OUTPUT" | grep "Parse time" | grep -Eo '[0-9]+\.[0-9]+' | head -1)
    QUERY=$(echo "$OUTPUT" | grep "Query time" | grep -Eo '[0-9]+\.[0-9]+' | head -1)
    TOTAL=$(echo "$OUTPUT" | grep "Total time" | grep -Eo '[0-9]+\.[0-9]+' | head -1)

    SUM_PARSE=$(awk "BEGIN {print $SUM_PARSE + $PARSE}")
    SUM_QUERY=$(awk "BEGIN {print $SUM_QUERY + $QUERY}")
    SUM_TOTAL=$(awk "BEGIN {print $SUM_TOTAL + $TOTAL}")
done

AVG_PARSE=$(awk "BEGIN {print $SUM_PARSE / 10}")
AVG_QUERY=$(awk "BEGIN {print $SUM_QUERY / 10}")
AVG_TOTAL=$(awk "BEGIN {print $SUM_TOTAL / 10}")

echo ""
echo "📊 RW,$AVG_PARSE,$AVG_QUERY,$AVG_TOTAL"
echo ""
echo "🖨️ Full output of all 10 runs:"
echo -e "$ALL_OUTPUTS"
