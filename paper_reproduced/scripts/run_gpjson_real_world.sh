#!/bin/bash -l

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=16
#SBATCH --output="result-gpjson-realworld-usecase.log"
#SBATCH --mem=16G
#SBATCH -p short_gpu
#SBATCH --gres=gpu:ada6000:1
#SBATCH --time=00:10:00

# Load needed modules
module load slurm
module load cuda/11.8


export GRAALVM_HOME=/rhome/aveda002/bigdata/gpjson/graalvm-ce-java8-21.0.0.2
export JAVA_HOME=$GRAALVM_HOME
export GRAALVM_DIR=$GRAALVM_HOME
export PATH=$GRAALVM_HOME/bin:$PATH

echo "🚀 Running GPJSON benchmark (10× per dataset)..."

declare -A SCRIPTS=(
  ["RW"]="../related_works/gpjson/gpjson-real-world.js"
)


# mkdir -p results
# OUT_FILE="results/gpjson_fig11.csv"
# : > "$OUT_FILE"
# echo "Dataset,AverageTime(ms)" > "$OUT_FILE"



for key in RW; do
  SCRIPT="${SCRIPTS[$key]}"
  echo "▶️  Running $key using $SCRIPT"

  sum=0
  for i in {1..10}; do
    # echo "  Run $i..."
    TIME_MS=$($GRAALVM_HOME/bin/node --polyglot --jvm "$SCRIPT" 2>&1 | grep "Execution time" | grep -Eo '[0-9]+\.[0-9]+')
    echo "  Run $i time: $TIME_MS ms"
    sum=$(awk "BEGIN {print $sum + $TIME_MS}")
  done

  avg=$(awk "BEGIN {print $sum / 10}")
  echo "$key average: $avg ms"
  # echo "$key,$avg" >> "$OUT_FILE"
done

echo "✅ GPJSON results saved"