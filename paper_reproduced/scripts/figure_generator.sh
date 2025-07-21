#!/bin/bash
set -e

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
