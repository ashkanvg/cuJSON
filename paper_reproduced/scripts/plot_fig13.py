import pandas as pd
import matplotlib.pyplot as plt

# -------------------------------
# Step 1: Load CSV
# -------------------------------
csv_path = "results/fig13_data.csv"
output_path = "figures/fig13.png"

df = pd.read_csv(csv_path)

# -------------------------------
# Step 2: Plot
# -------------------------------
# Set the figure size
plt.figure(figsize=(10, 6))

# Define the components to be stacked
components = ['h2d', 'validation', 'tokenization', 'parsing', 'd2h']

# Plot the stacked bar chart
ax = df.set_index('Dataset')[components].plot(kind='bar', stacked=True, width=0.8, color=plt.cm.Paired.colors)

# Customize the plot
plt.title("Figure 13: Time Breakdown for Each Dataset")
plt.xlabel("Dataset")
plt.ylabel("Time (ms)")
plt.xticks(rotation=45)
plt.tight_layout()

# Add a legend with labels from the components
plt.legend(title="Steps", labels=components, bbox_to_anchor=(1.05, 1), loc='upper left')

# Save the figure
plt.savefig(output_path)

print(f"✅ Figure saved to {output_path}")
