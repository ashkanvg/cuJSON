#!/bin/bash -l

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=8
#SBATCH --output="result-cudf-twitter.log"
#SBATCH --mem=16G
#SBATCH -p gpu
#SBATCH --gres=gpu:a100:1
#SBATCH --time=00:10:00

# Load needed modules
module load slurm
module load cuda/11.8


conda list cudf



echo "🚀 Running cuDF benchmarks (10× each) and printing full output..."

# -------------------------------
# cuDF Benchmark Scripts (Python)
# -------------------------------
cd ../related_works/cuDF/

declare -A SCRIPTS=(
  ["RW"]="twitter.py"
)

# Run each script 10× and show all output
for key in RW; do
  SCRIPT="${SCRIPTS[$key]}"
  echo "▶️  Running $key with $SCRIPT"

  sum=0
  for i in {1..10}; do
    echo "🔁 Run #$i:"
    
    # Capture full output
    OUTPUT=$(python "$SCRIPT")
    echo "$OUTPUT"

    # Extract just the "Time taken to parse" value from the output for averaging
    TIME_MS=$(echo "$OUTPUT" | grep "Time taken to parse:" | grep -Eo '[0-9]+\.[0-9]+')
    sum=$(awk "BEGIN {print $sum + $TIME_MS}")
  done

  avg=$(awk "BEGIN {print $sum / 10}")
  echo "📊 $key average parse time: $avg ms"
done

echo "✅ cuDF full output printed. Done!"
