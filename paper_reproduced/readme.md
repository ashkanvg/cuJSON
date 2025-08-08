# cuJSON: A Highly Parallel JSON Parser for GPUs [Paper Reproduced]

## Important Notes for Paper Reproducibility
This readme is for use of paper reproduce manually, but in the case if you want to use the codes with automatic scripts and all the related compile and run codes in the `scripts` folder (with its own `readme.md`). All the information of reporducing all the figures are existed in that directory.

## Datasets
Two sample datasets are included in the `dataset` folder. Large datasets (used in performance evaluation) can be downloaded from https://drive.google.com/drive/folders/1PkDEy0zWOkVREfL7VuINI-m9wJe45P2Q?usp=sharing and placed into the `dataset` folder. 

- For JSON Lines, use those datasets that end in `_small_records.json`. 
- For Standard JSON, use those datasets that end in `_large_record.json`.

## Prerequisites: 
- `g++` (version 7 or better), 
- `Cuda` compilation tools (release 12.1), 
- and a 64-bit system with a command-line shell (e.g., Linux, macOS, FreeBSD). 

<hr>

## Reproduce the Results of Paper
We provided produced results and figures (all of the results that this script can reproduce) at the end of this section. 
Here, we provided all results of all figures by direct compile and run our code based on the Prerequisites: 
1. Figure 9/10:           Parsing Time of Standard JSON
2. Figure 11:             Parsing Time of JSON Lines
3. Figure 12:             Peak GPU Memory Footprint
4. Figure 13/ Table 8:    Time Breakdown of cuJSON
5. Figure 14:             Space Cost of Parsing Output
6. Figure 15:             Querying Cost 


- Notes: Figure numbers are based on the papers figure numbers. In `scripts` directory we provide the python code to generate the figures. 


### [1, 4, and 5] - Standard JSON (One Large JSON Record)
The cuJSON library is easily consumable. 
1. clone the repo in your directory and move to `paper_reproduced/` directory.
2. follow the following command to compile your code: 

```
nvcc -O3 -o output_large.exe ./src/cuJSON-standardjson.cu -w [-gencode=arch=compute_61,code=sm_61]
```

**NOTE**: We set the buffer size to filesize in this file.

3. Download the corresponding JSON files from the provided dataset URL and copy the downloaded file to the `dataset` folder. Then, use this command line to parse it (default version).

```
output_large.exe -b ../dataset/[dataset name]_small_records.json
```

**NOTE**: Possible [dataset name]s are {`nspl`, `wiki`, `walmart`, `google_map`, `twitter`, `bestbuy`}.

4. Your results are ready. It will print out the following results:
```
Batch mode running...
1. H2D:                 [host to device time in ms, reported in Figure 13/14]
2. Validation:          [validation time in ms, reported in Figure 13/14]
3. Tokenization:        [tokenization time in ms, reported in Figure 13/14]
4. Parser:              [parser time in ms, reported in Figure 13/14]
5. D2H:                 [device to host time in ms, reported in Figure 13/14]

TOTAL (ms):             [total time in ms, reported in Figure 7/8]

Parser's Output Size:   [output memory allocation in MB, reported in Figure 11]
```

### [2, 4, and 5] - JSON Lines (JSON Records that are separated by newline)
The cuJSON library is easily consumable. 
1. clone the repo in your directory and move to `paper_reproduced/` directory. 
2. follow the following command to compile your code: 

```
nvcc -O3 -o output_small.exe ./src/cuJSON-jsonlines.cu -w [-gencode=arch=compute_61,code=sm_61]
```

**NOTE**: In this file, the buffer size is set to 256MB, but you can change it in the code by changing `#define BUFSIZE  268435456`.


3. Download the corresponding JSON files from the provided dataset URL and copy the downloaded file to the `dataset` folder. Then, use this command line to parse it (default version).

```
output_small.exe -b ../dataset/[dataset name]_small_records.json
```

**NOTE**: Possible [dataset name]s are {`nspl`, `wiki`, `walmart`, `google_map`, `twitter`, `bestbuy`}.

4. Your results are ready. It will print out the following results:
```
Batch mode running...
1. H2D:                 [host to device time in ms, reported in Figure 13/14]
2. Validation:          [validation time in ms, reported in Figure 13/14]
3. Tokenization:        [tokenization time in ms, reported in Figure 13/14]
4. Parser:              [parser time in ms, reported in Figure 13/14]
5. D2H:                 [device to host time in ms, reported in Figure 13/14]

TOTAL (ms):             [total time in ms, reported in Figure 9/10]

Parser's Output Size:   [output memory allocation in MB, reported in Figure 11]
```
**NOTE**: In paper, for JSON Lines, we report the total time without D2H time. Here, we are reporting the Total time by considering D2H computation.

<hr>


### [3] - Peak GPU Memory
We use the following terminal command to report used gpu memeory:
```
nvidia-smi --query-gpu=memory.used --format=csv,noheader,nounits | head -n 1
```

However, we have to follow the following steps. 
1. Repeat the same procedure of "[2, 4, and 6] - JSON Lines" until step 3.
2. For step 3 follow the following bash script:

```
# Initialize the output log for CPU and GPU memory usage
cpu_gpu_log="cpu_gpu_usage.log"
echo "Timestamp,Device,Memory_Used" > $cpu_gpu_log

# Function to log CPU memory usage
log_cpu_memory() {
    local pid=$$
    local page_size=$(getconf PAGESIZE)
    local rss_pages=$(awk '{print $2}' /proc/$pid/statm)
    local memory_kb=$((rss_pages * page_size / 1024))
    echo "$(date '+%Y-%m-%d %H:%M:%S.%3N'),CPU,${memory_kb} KB" >> $cpu_gpu_log
}

# Function to log GPU memory usage
log_gpu_memory() {
    local timestamp=$(date '+%Y-%m-%d %H:%M:%S.%3N')
    local gpu_mem=$(nvidia-smi --query-gpu=memory.used --format=csv,noheader,nounits | head -n 1)
    echo "${timestamp},GPU,${gpu_mem} MiB" >> $cpu_gpu_log
}

# Start CPU and GPU memory logging in the background
while true; do
    log_cpu_memory
    log_gpu_memory
    sleep 0.001  # Log every 0.001s
done &
logging_pid=$!


output_small.exe -b ./dataset/[dataset name]_small_records.json

kill $logging_pid
```

**NOTE**: Possible [dataset name]s are {`nspl`, `wiki`, `walmart`, `google_map`, `twitter`, `bestbuy`}.



### [6] - Query Time + Real World Example
In order to run all of the queries, please follow the following steps. We are reporting for query time of each record. In the paper, we report average time and also aggregated time of calling each query. 

1. Clone the repo in your directory and move to `paper_reproduced/` directory.
2. Download the corresponding JSON files from the provided dataset URL and copy the downloaded file to the `../dataset` folder.
3. Run the following bash script to compile and run all of the queries. 

```
echo 'TT1:';
nvcc -O3 -o query-experiment ./example/query_TT1_JSONL.cu -w -gencode=arch=compute_61,code=sm_61;
./query-experiment -b ./dataset/twitter_small_records.json;

echo 'TT2:';
nvcc -O3 -o query-experiment ./example/query_TT2_JSONL.cu -w -gencode=arch=compute_61,code=sm_61;
./query-experiment -b ./dataset/twitter_small_records.json;

echo 'TT3:';
nvcc -O3 -o query-experiment ./example/query_TT3_JSONL.cu -w -gencode=arch=compute_61,code=sm_61;
./query-experiment -b ./dataset/twitter_small_records.json;

echo 'TT4:';
nvcc -O3 -o query-experiment ./example/query_TT4_JSONL.cu -w -gencode=arch=compute_61,code=sm_61;
./query-experiment -b ./dataset/twitter_small_records.json;

echo 'WM:';
nvcc -O3 -o query-experiment ./example/query_WM_JSONL.cu -w -gencode=arch=compute_61,code=sm_61;
./query-experiment -b ./dataset/walmart_small_records.json ;

echo 'GMD1:';
nvcc -O3 -o query-experiment ./example/query_GMD1_JSONL.cu -w -gencode=arch=compute_61,code=sm_61;
./query-experiment -b ./dataset/google_map_small_records.json ;

echo 'GMD2:';
nvcc -O3 -o query-experiment ./example/query_GMD2_JSONL.cu -w -gencode=arch=compute_61,code=sm_61;
./query-experiment -b ./dataset/google_map_small_records.json ;

echo 'nspl:';
nvcc -O3 -o query-experiment ./example/query_NSPL_JSONL.cu -w -gencode=arch=compute_61,code=sm_61;
./query-experiment -b ./dataset/nspl_small_records.json;

echo 'BB1:';
nvcc -O3 -o query-experiment ./example/query_BB1_JSONL.cu -w -gencode=arch=compute_61,code=sm_61;
./query-experiment -b ./dataset/bestbuy_small_records.json ;

echo 'BB2:';
nvcc -O3 -o query-experiment ./example/query_BB2_JSONL.cu -w -gencode=arch=compute_61,code=sm_61;
./query-experiment -b ./dataset/bestbuy_small_records.json ;

echo 'WP1:';
nvcc -O3 -o query-experiment ./example/query_WP1_JSONL.cu -w -gencode=arch=compute_61,code=sm_61;
./query-experiment -b ./dataset/wiki_small_records.json ;

echo 'WP2:';
nvcc -O3 -o query-experiment ./example/query_WP2_JSONL.cu -w -gencode=arch=compute_61,code=sm_61;
./query-experiment -b ./dataset/wiki_small_records.json ;

```


## Generate Figures
In order to generate the figures we use python codes that are located in `scripts` directory. Our pipelines is contains:
1. run the scripts to generate .csv files 
2. run python code over those .csv to generate those pictures.




## Related Works
We also provided instructions on running the related works and the methods we used to compare the cuJSON with them. 
Here is a list of the directories to their corresponding instruction:

1. cuDF [JSON Lines]: `./related_wroks/cuDF`
2. GPJSON [JSON Lines]: `./related_wroks/gpjson`
3. pison [JSON Lines, standarad JSON]: `./related_wroks/pison`
4. rapidJSON [standard JSON]: `./related_wroks/rapidjson`
5. simdjson [JSON Lines, standarad JSON]: `./related_wroks/simdjson`
5. MetaJSON [JSON Lines]: `./related_wroks/metajson`


<hr>