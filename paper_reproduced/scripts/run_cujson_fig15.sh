#!/bin/bash

echo "🚀 Running all query experiments (10× each) and computing averages..."

mkdir -p results
OUT_FILE="results/cujson_fig15_temp.csv"
: > "$OUT_FILE"

OUT_FILE_FINAL="results/cujson_fig15.csv"
: > "$OUT_FILE_FINAL"
echo "QueryName,AverageTime" >> "$OUT_FILE"

# 🔧 Full path to directory containing query_XYZ_JSONL.cu
SRC_DIR="../query_example/"

declare -A QUERIES=(
  ["TT1"]="query_TT1_JSONL.cu ../../dataset/twitter_small_records.json"
  ["TT2"]="query_TT2_JSONL.cu ../../dataset/twitter_small_records.json"
  ["TT3"]="query_TT3_JSONL.cu ../../dataset/twitter_small_records.json"
  ["TT4"]="query_TT4_JSONL.cu ../../dataset/twitter_small_records.json"
  ["WM"]="query_WM_JSONL.cu ../../dataset/walmart_small_records.json"
  ["GMD1"]="query_GMD1_JSONL.cu ../../dataset/google_map_small_records.json"
  ["GMD2"]="query_GMD2_JSONL.cu ../../dataset/google_map_small_records.json"
  ["NSPL"]="query_NSPL_JSONL.cu ../../dataset/nspl_small_records.json"
  ["BB1"]="query_BB1_JSONL.cu ../../dataset/bestbuy_small_records.json"
  ["BB2"]="query_BB2_JSONL.cu ../../dataset/bestbuy_small_records.json"
  ["WP1"]="query_WP1_JSONL.cu ../../dataset/wiki_small_records.json"
)

declare -A group_sums
declare -A group_counts

for key in "${!QUERIES[@]}"; do
  IFS=' ' read -r cu_file json_file <<< "${QUERIES[$key]}"
  echo "🔹 Running..."

  # Compile from absolute path
  nvcc -O3 -o query-experiment "$SRC_DIR/$cu_file" -w -gencode=arch=compute_89,code=sm_89

  sum=0
  for i in {1..10}; do
    TIME=$(./query-experiment -b "$json_file" | grep -Eo '[0-9]+(\.[0-9]+)?' | tail -1)
    sum=$(awk "BEGIN {print $sum + $TIME}")
  done

  avg=$(awk "BEGIN {print $sum / 10}")
  # echo "$key average: $avg ns"
  echo "$key,$avg" >> "$OUT_FILE"

  # Group accumulation
  prefix=$(echo "$key" | grep -Eo '^[A-Z]+')
  group_sums["$prefix"]=$(awk "BEGIN {print ${group_sums[$prefix]:-0} + $avg}")
  group_counts["$prefix"]=$((${group_counts[$prefix]:-0} + 1))
done

# Group-wise summary
# -----------------------------
echo "" >> "$OUT_FILE"
echo "Group,GroupAverage" >> "$OUT_FILE"
echo ""
echo "📊 Group-wise Averages:"
for prefix in "${!group_sums[@]}"; do
  total=${group_sums[$prefix]}
  count=${group_counts[$prefix]}
  group_avg=$(awk "BEGIN {print $total / $count}")
  echo "$prefix → $group_avg ns"
  echo "$prefix,$group_avg" >> "$OUT_FILE"
done

# -----------------------------
# Final total average of all groups
# -----------------------------
total_sum=0
group_count=0

while IFS=',' read -r group avg; do
  # Skip header
  if [[ "$group" == "Group" ]]; then continue; fi
  total_sum=$(awk "BEGIN {print $total_sum + $avg}")
  group_count=$((group_count + 1))
done < <(tail -n +$(($(grep -n '^Group,GroupAverage' "$OUT_FILE" | cut -d: -f1) + 1)) "$OUT_FILE")

final_avg=$(awk "BEGIN {print $total_sum / $group_count}")
echo ""
echo "🌐 Final overall average of all group averages: $final_avg ns"
echo "AVERAGE,$final_avg" >> "$OUT_FILE_FINAL"
