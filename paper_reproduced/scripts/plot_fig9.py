import pandas as pd
import matplotlib.pyplot as plt
import sys

# -------------------------------
# Step 1: Load CSV
# -------------------------------
csv_path = "results/fig9_data.csv"
output_path = "results/fig9.png"

df = pd.read_csv(csv_path)

# -------------------------------
# Step 2: Plot
# -------------------------------
plt.figure(figsize=(10, 6))
bar_width = 0.2
x = range(len(df["Dataset"]))

# Plot each method with an offset
plt.bar([i - 1.5*bar_width for i in x], df["cuJSON"], width=bar_width, label="cuJSON")
plt.bar([i - 0.5*bar_width for i in x], df["simdjson"], width=bar_width, label="simdjson")
plt.bar([i + 0.5*bar_width for i in x], df["RapidJSON"], width=bar_width, label="RapidJSON")
plt.bar([i + 1.5*bar_width for i in x], df["Pison"], width=bar_width, label="Pison")

plt.xticks(x, df["Dataset"])
plt.ylabel("Parsing Time (ms)")
plt.title("Figure 9: End-to-End Parsing Time on Large Datasets")
plt.legend()
plt.grid(axis='y', linestyle='--', alpha=0.5)
plt.tight_layout()
plt.savefig(output_path)

print(f"✅ Figure saved to {output_path}")
