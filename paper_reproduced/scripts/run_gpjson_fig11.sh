#!/bin/bash -l

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=16
#SBATCH --output="result-GPU-gpjson-2.log"
#SBATCH --mem=32G
#SBATCH -p gpu
#SBATCH --gres=gpu:a100:1
#SBATCH --time=01:0:00

module load slurm
module unload java
module load cuda/11.8

export GRAALVM_HOME=/rhome/aveda002/bigdata/gpjson/graalvm-ce-java8-21.0.0.2
export JAVA_HOME=$GRAALVM_HOME
export GRAALVM_DIR=$GRAALVM_HOME
export PATH=$GRAALVM_HOME/bin:$PATH

set -e

echo "🚀 Running GPJSON benchmark (10× per dataset)..."

declare -A SCRIPTS=(
  ["TT"]="/rhome/aveda002/bigdata/cuJSON/paper_reproduced/related_works/gpjson/gpjson-twitter.js"
  ["BB"]="/rhome/aveda002/bigdata/cuJSON/paper_reproduced/related_works/gpjson/gpjson-bestbuy.js"
  ["GMD"]="/rhome/aveda002/bigdata/cuJSON/paper_reproduced/related_works/gpjson/gpjson-google.js"
  ["NSPL"]="/rhome/aveda002/bigdata/cuJSON/paper_reproduced/related_works/gpjson/gpjson-nspl.js"
  ["WM"]="/rhome/aveda002/bigdata/cuJSON/paper_reproduced/related_works/gpjson/gpjson-walmart.js"
  ["WP"]="/rhome/aveda002/bigdata/cuJSON/paper_reproduced/related_works/gpjson/gpjson-wiki.js"
)


mkdir -p results
OUT_FILE="results/gpjson_fig11_tmp.csv"
: > "$OUT_FILE"
echo "Dataset,AverageTime(ms)" > "$OUT_FILE"



for key in TT BB GMD NSPL WM WP; do
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
  echo "$key,$avg" >> "$OUT_FILE"
done

echo "✅ GPJSON results saved to $OUT_FILE"