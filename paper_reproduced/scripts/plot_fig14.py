import pandas as pd
import matplotlib.pyplot as plt

# -------------------------------
# Step 1: Load CSV
# -------------------------------
csv_path = "results/fig14_data.csv"
output_path = "figures/fig14.png"

df = pd.read_csv(csv_path)

# -------------------------------
# Step 2: Plot
# -------------------------------
plt.figure(figsize=(12, 6))
bar_width = 0.15
x = range(len(df["Dataset"]))

# Plot each method with an offset
plt.bar([i - 2*bar_width for i in x], df["cuJSON"], width=bar_width, label="cuJSON")
plt.bar([i - bar_width for i in x], df["cuDF"], width=bar_width, label="cuDF")
plt.bar([i for i in x], df["simdjson"], width=bar_width, label="simdjson")
plt.bar([i + bar_width for i in x], df["pison"], width=bar_width, label="pison")
plt.bar([i + 2*bar_width for i in x], df["rapidjson"], width=bar_width, label="rapidjson")

# Customize the plot
plt.xticks(x, df["Dataset"])
plt.ylabel("Output Memory Usage (MB)")
plt.title("Figure 14: Output Memory Usage")
plt.legend()
plt.grid(axis='y', linestyle='--', alpha=0.5)
plt.tight_layout()

# Save the figure
plt.savefig(output_path)

print(f"✅ Figure saved to {output_path}")
