#!/bin/bash -l

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4
#SBATCH --output="result-GPU.log"
#SBATCH --mem=8G
#SBATCH -p short_gpu
#SBATCH --gres=gpu:ada6000:1
#SBATCH --time=01:00:00


# Load needed modules
module load slurm
module load cuda/11.8

# find . -name "json.cpython-*.so" -delete
conda activate cuDF_env

#!/bin/bash
set -e

# ------------------------------
# Step 1: Input CSV paths
# ------------------------------
CUJSON="results/cujson_fig15.csv"
simdjson="results/simdjson_fig15.csv"
pison="results/pison_fig15.csv"
rapidjson="results/rapidjson_fig15.csv"
MERGED="results/merged_fig15.csv"

# ------------------------------
# Step 2: Output header
# ------------------------------
echo "Method,cuJSON,simdjson,pison,rapidjson" > "$MERGED"

# ------------------------------
# Step 3: Extract and merge AVERAGE values line-by-line
# ------------------------------
# Extract the "AVERAGE" values for each method from the CSVs and merge them
cu_val=$(grep "^AVERAGE" "$CUJSON" | cut -d',' -f2)
simdjson_val=$(grep "^AVERAGE" "$simdjson" | cut -d',' -f2)
pison_val=$(grep "^AVERAGE" "$pison" | cut -d',' -f2)
rapidjson_val=$(grep "^AVERAGE" "$rapidjson" | cut -d',' -f2)

# Write the results for the "AVERAGE" key
echo "AVERAGE,$cu_val,$simdjson_val,$pison_val,$rapidjson_val" >> "$MERGED"

echo "✅ Combined results saved to $MERGED"
