import pandas as pd
import matplotlib.pyplot as plt

# -------------------------------
# Step 1: Load CSV
# -------------------------------
csv_path = "results/fig11_data.csv"
output_path = "figures/fig11.png"

df = pd.read_csv(csv_path)

# -------------------------------
# Step 2: Plot
# -------------------------------
plt.figure(figsize=(10, 6))
bar_width = 0.2
x = range(len(df["Dataset"]))

# Plot each method with an offset
plt.bar([i - 1.5*bar_width for i in x], df["cuJSON"], width=bar_width, label="cuJSON")
plt.bar([i - 0.5*bar_width for i in x], df["cuDF"], width=bar_width, label="cuDF")
plt.bar([i + 0.5*bar_width for i in x], df["GPJSON"], width=bar_width, label="GPJSON")

plt.xticks(x, df["Dataset"])
plt.ylabel("Parsing Time (ms)")
plt.title("Figure 11: End-to-End Parsing Time")
plt.legend()
plt.grid(axis='y', linestyle='--', alpha=0.5)
plt.tight_layout()
plt.savefig(output_path)

print(f"✅ Figure saved to {output_path}")
