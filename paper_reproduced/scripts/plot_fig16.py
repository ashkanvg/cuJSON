import pandas as pd
import matplotlib.pyplot as plt
import os

# -------------------------------
# Step 1: Load CSV
# -------------------------------
csv_path = "results/fig16_data.csv"
output_dir = "figures"
output_path = os.path.join(output_dir, "fig16.png")

# Ensure the output directory exists
os.makedirs(output_dir, exist_ok=True)

# Read the CSV
df = pd.read_csv(csv_path)

# Set the first column as index (dataset names)
df.set_index("Dataset", inplace=True)
df = df.T  # Transpose to have sizes on x-axis
df.index = df.index.astype(float)  # Ensure numeric index for plotting

# -------------------------------
# Step 2: Plot
# -------------------------------
plt.figure(figsize=(10, 6))
for column in df.columns:
    plt.plot(df.index, df[column], marker='o', label=column.upper())

plt.xscale('log', base=2)
plt.yscale('log')
plt.xticks(df.index, labels=[str(int(x)) for x in df.index])
plt.xlabel("JSON Data Size (MB)")
plt.ylabel("Parsing Time (ms)")
plt.title("Figure 16. Scalability of cuJSON (Standard JSON, Server)")
plt.grid(axis='y', linestyle='--', alpha=0.5)
plt.legend()
plt.tight_layout()

# Save the figure
plt.savefig(output_path)
print(f"✅ Figure saved to {output_path}")
