#!/bin/bash -l

#SBATCH --nodes=1
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4
#SBATCH --output="result-GPU-cuDF-wiki.log"
#SBATCH --mem=10G
#SBATCH -p gpu
#SBATCH --gres=gpu:a100:1
#SBATCH --time=00:05:00

# Load needed modules
module load slurm
module load cuda/11.8

# find . -name "json.cpython-*.so" -delete
conda activate cuDF_env

echo '1:';
nvcc --version
echo '2:';
nvidia-smi

# echo 'test';
# which python;


# # Initialize the output log for CPU and GPU memory usage
# cpu_gpu_log="cpu_gpu_usage.log"
# echo "Timestamp,Device,Memory_Used" > $cpu_gpu_log

# # Function to log CPU memory usage
# log_cpu_memory() {
#     local pid=$$
#     local page_size=$(getconf PAGESIZE)
#     local rss_pages=$(awk '{print $2}' /proc/$pid/statm)
#     local memory_kb=$((rss_pages * page_size / 1024))
#     echo "$(date '+%Y-%m-%d %H:%M:%S.%3N'),CPU,${memory_kb} KB" >> $cpu_gpu_log
# }

# # Function to log GPU memory usage
# log_gpu_memory() {
#     local timestamp=$(date '+%Y-%m-%d %H:%M:%S.%3N')
#     local gpu_mem=$(nvidia-smi --query-gpu=memory.used --format=csv,noheader,nounits | head -n 1)
#     echo "${timestamp},GPU,${gpu_mem} MiB" >> $cpu_gpu_log
# }

# # Start CPU and GPU memory logging in the background
# while true; do
#     log_cpu_memory
#     log_gpu_memory
#     sleep 0.001  # Log every 0.001s
# done &
# logging_pid=$!

echo '___________________________________________________________'
cd ../related_works/cuDF;
# python -m trace --trace ./main.py
# python -v ./main.py > trace.log 2>&1
# /usr/bin/time -v python ./main.py;
# /usr/bin/time -v python ./main-large.py;
python ./wiki.py;
# /usr/bin/time -v python ./wiki.py;


# Clean up logging processes after execution
kill $logging_pid