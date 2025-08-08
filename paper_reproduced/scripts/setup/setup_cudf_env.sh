#!/bin/bash -l

module purge
module load miniconda3
module load cuda/12.8

conda create -n cudf_env -c rapidsai -c conda-forge cudf=25.08

echo "Conda environment for cudf created"
echo "Run 'conda activate cudf_env' to activate"

