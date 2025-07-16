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
CUJSON="results/cujson_fig9_temp.csv"
cudf="results/cudf_fig11_tmp.csv"
gpjson="results/gpjson_fig11_tmp.csv"
MERGED="results/fig11_data.csv"

# Ordered list of datasets (enforces row order)
ORDERED_KEYS=("TT" "BB" "GMD" "NSPL" "WM" "WP")

# ------------------------------
# Step 2: Output header
# ------------------------------
echo "Dataset,cuJSON,cuDF,GPJSON" > "$MERGED"

# ------------------------------
# Step 3: Merge values line-by-line
# ------------------------------
for dataset in "${ORDERED_KEYS[@]}"; do
    cu_val=$(grep "^$dataset," "$CUJSON" | cut -d',' -f2)
    cudf_val=$(grep "^$dataset," "$cudf" | cut -d',' -f2)
    gpjson_val=$(grep "^$dataset," "$gpjson" | cut -d',' -f2)

    echo "$dataset,$cu_val,$cudf_val,$gpjson_val" >> "$MERGED"
done

echo "✅ Combined results saved to $MERGED"
