#!/bin/bash -l

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4
#SBATCH --output="result-figure-generation.log"
#SBATCH --mem=8G
#SBATCH -p short_gpu
#SBATCH --gres=gpu:ada6000:1
#SBATCH --time=01:00:00

# -------------------------------
# Step 1: Load necessary modules
# -------------------------------
module load slurm
module load cuda/11.8

# -------------------------------
# Step 3: Run the external Python script to generate the figure
# -------------------------------
python plot_fig9.py
python plot_fig11.py
python plot_fig12.py
python plot_fig13.py
python plot_fig14.py
python plot_fig15.py
python plot_fig16.py

echo "✅ Python script completed and figure generated!"
