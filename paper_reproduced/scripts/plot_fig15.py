import pandas as pd
import matplotlib.pyplot as plt

# -------------------------------
# Step 1: Load CSV
# -------------------------------
csv_path = "results/fig15_data.csv"  # Assuming the file name is fig15_data.csv
output_path = "figures/fig15.png"

df = pd.read_csv(csv_path)

# -------------------------------
# Step 2: Plot
# -------------------------------
plt.figure(figsize=(10, 6))


# Bar width
bar_width = 0.2

# X positions for each method
x = range(len(df["Method"]))

# Plot each method as individual bars
bars1 = plt.bar([i - 1.5*bar_width for i in x], df["cuJSON"], width=bar_width, label="cuJSON")
bars2 = plt.bar([i - 0.5*bar_width for i in x], df["simdjson"], width=bar_width, label="simdjson")
bars3 = plt.bar([i + 0.5*bar_width for i in x], df["pison"], width=bar_width, label="pison")
bars4 = plt.bar([i + 1.5*bar_width for i in x], df["rapidjson"], width=bar_width, label="rapidjson")

# Customize the plot
plt.xticks(x, df["Method"])
plt.ylabel("Time (ns)")
plt.title("Figure 15: Single Query Return Time")
plt.legend()

# Display the value of each bar on top of it
for bars in [bars1, bars2, bars3, bars4]:
    for bar in bars:
        height = bar.get_height()
        plt.text(bar.get_x() + bar.get_width() / 2, height + 20, f'{height:.2f}', ha='center', va='bottom')

# Tighten layout and save the figure
plt.tight_layout()
plt.savefig(output_path)

print(f"✅ Figure saved to {output_path}")