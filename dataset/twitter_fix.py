import json

input_file = "twitter_small_records.json"
output_file = "twitter_clean_final.json"

with open(input_file, "r") as infile, open(output_file, "w") as outfile:
    buffer = ""
    for line in infile:
        buffer += line.strip()
        if buffer.endswith("}"):
            try:
                obj = json.loads(buffer)
                json.dump(obj, outfile)
                outfile.write("\n")
                buffer = ""
            except json.JSONDecodeError:
                # Incomplete object, keep reading lines
                buffer += " "

print(f"Saved clean JSONL to {output_file}")
