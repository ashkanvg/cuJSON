#!/bin/bash

# Run all figure scripts sequentially
./run_all_fig9.sh
./run_all_fig11.sh
./run_all_fig12.sh
./run_all_fig14.sh
./run_all_fig15.sh
./run_cujson_fig16.sh

# -------------------------------
# Run the external Python script to generate the figure
# -------------------------------
python plot_fig9.py
python plot_fig11.py
python plot_fig12.py
python plot_fig13.py
python plot_fig14.py
python plot_fig15.py
python plot_fig16.py

echo "✅ Python script completed and figure generated!"
