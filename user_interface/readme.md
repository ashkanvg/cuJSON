# cuJSON: Parsing Large JSON Data on a GPU
JSON (JavaScript Object Notation) has become a ubiquitous data format in modern computing, but its parsing can be a significant performance bottleneck. Contrary to the conventional wisdom that GPUs are not suitable for parsing due to the branching-rich nature of parsing algorithms, this work presents a novel GPU-based JSON parser, cuJSON, that redesigns the parsing algorithm to minimize branches and optimizes it for GPU architectures.

Inspired by recent advances in SIMD-based JSON processing, our work concentrates on exploiting bitwise parallelism, leveraging GPU intrinsic functions and high-performance CUDA libraries optimally. We introduce a novel output data structure that balances parsing and querying costs, and implement innovative techniques to break key dependencies in the parsing process. Through extensive experimentation, our evaluations demonstrate that cuJSON not only surpasses traditional CPU-based JSON parsers (like simdjson and Pison) but also outperforms existing GPU-based JSON parsers (such as cuDF and GPJSON), achieving unparalleled parsing speeds.

<hr>

## Datasets
Two sample datasets are included in the `dataset` folder. Large datasets (used in performance evaluation) can be downloaded from https://drive.google.com/drive/folders/1PkDEy0zWOkVREfL7VuINI-m9wJe45P2Q?usp=sharing and placed into the `dataset` folder. For JSON Lines, use those datasets that end in `_small_records.json`. Each dataset comes with two formats:

- For JSON Lines, use those datasets that end in `_small_records.json`. 
- For Standard JSON, use those datasets that end in `_large_record.json`.

## Prerequisites: 
- `g++` (version 7 or better), 
- `Cuda` compilation tools (release 12.1), 
- and a 64-bit system with a command-line shell (e.g., Linux, macOS, FreeBSD). 

<hr>

## Reproduce the Results
Here, we provided two different ways to compile and run our code based on the Prerequisites: 
1. with Makefile
2. direct compile and run

We provided produced results and figures (all of the results that this script can reproduce) at the end of this section. 

### 1. Quick Start [phase times, total time, output size] - Makefile
The cuJSON library is easily consumable. 
1. clone the repo in your directory. 
2. Download Dataset: Download all the datasets from https://drive.google.com/drive/folders/1PkDEy0zWOkVREfL7VuINI-m9wJe45P2Q?usp=sharing and place them in the `dataset` folder.
3. Compile the project for both Standard JSON and JSON Lines datasets:
```
make -f Makefile.compile
```

**NOTE**: 
You can change the `Makefile.compile` based on your system. These are the possible modifications:
- Change the `nvcc` path based on your system.
- You must set the `gencode` based on your NVIDIA GPU. Our default `gencode` is based on our desktop: `-gencode=arch=compute_61,code=sm_61`

4. Run: 
- if you are looking for JSON Lines (JSON Records that are separated by newline)
```
make -f Makefile.run run_small SMALL_DATASETS="custom_dataset1_small_records.json custom_dataset2_small_records.json"
```
**NOTE**: In this file, we set the buffer size to 256MB, but you can change it in the code by changing `#define BUFSIZE  268435456`.

If you are looking for Standard JSON (One Large JSON Record), the buffer size in this file is equal to the file size.
```
make -f Makefile.run run_large LARGE_DATASETS="custom_dataset1_large_record.json custom_dataset2_large_record.json"
```

5. Your results are ready. It will print out the following results (for each dataset):
```
Batch mode running...
1. H2D:                 [host to device time in ms]
2. Validation:          [validation time in ms]
3. Tokenization:        [tokenization time in ms]
4. Parser:              [parser time in ms]
5. D2H:                 [device to host time in ms]

TOTAL (ms):             [total time in ms]

Parser's Output Size:   [output memory allocation in MB]
```

6. Cleaning Compiled Binaries: If you want to clean the compiled binaries, use:
```
make -f Makefile.compile clean
```


### 2. Quick Start [phase times, total time, output size] - direct compile and run
The cuJSON library is easily consumable. 
1. clone the repo in your directory. 
2. follow the following command to compile your code: 

- if you are looking for JSON Lines (JSON Records that are separated by newline)

```
nvcc -O3 -o output_small.exe ./src/cuJSON-jsonlines.cu -w [-gencode=arch=compute_61,code=sm_61]
```

**NOTE**: In this file, the buffer size is set to 256MB, but you can change it in the code by changing `#define BUFSIZE  268435456`.

If you are looking for Standard JSON (One Large JSON Record), the buffer size in this file is equal to the file size.

```
nvcc -O3 -o output_large.exe ./src/cuJSON-standardjson.cu -w [-gencode=arch=compute_61,code=sm_61]
```
**NOTE**: We set the buffer size to filesize in this file.


3. Download the corresponding JSON files from the provided dataset URL and copy the downloaded file to the `dataset` folder. Then, use this command line to parse it (default version).

- if you are looking for JSON Lines (JSON Records that are separated by newline)

```
output_small.exe -b ./dataset/[dataset name]_small_records_remove.json
```

If you are looking for Standard JSON (One Large JSON Record), the buffer size in this file is equal to the file size.

```
output_large.exe -b ./dataset/[dataset name]_small_records_remove.json
```

**NOTE**: Possible [dataset name]s are {`nspl`, `wiki`, `walmart`, `google_map`, `twitter`, `bestbuy`}.

4. Your results are ready. It will print out the following results:
```
Batch mode running...
1. H2D:                 [host to device time in ms]
2. Validation:          [validation time in ms]
3. Tokenization:        [tokenization time in ms]
4. Parser:              [parser time in ms]
5. D2H:                 [device to host time in ms]

TOTAL (ms):             [total time in ms]

Parser's Output Size:   [output memory allocation in MB]
```

<hr>

## Related Works
We also provided instructions on running the related works and the methods we used to compare the cuJSON with them. 
Here is a list of the directories to their corresponding instruction:

1. cuDF [JSON Lines]: `./related_wroks/cuDF`
2. GPJSON [JSON Lines]: `./related_wroks/gpjson`
3. pison [JSON Lines, standarad JSON]: `./related_wroks/pison`
4. rapidJSON [standard JSON]: `./related_wroks/rapidjson`
5. simdjson [JSON Lines, standarad JSON]: `./related_wroks/simdjson`


<hr>


## Performance Results
- We compared cuJSON with cuDF, GJSON, Pison, RapidJSON, and simdjson for processing (i) a single JSON object/array (standard JSON) and (ii) a sequence of small JSON records (JSON Lines). 
- These datasets include National Statistics Postcode Lookup (NSPL) data for the UK, Tweets (TT), Walmart (WM) products, Wikipedia (WP) entities, Google Map Directions(GMD), and Best Buy (BB) products. Each dataset is a single large JSON record of approximately 1GB.
- All experiments were conducted on two machines: (i) Desktop [Intel Xeon E3-1225 V6][Nvidia Quadro P4000] (ii) Server [AMD EPYC 7713][Nvidia A100]

- These are the results that can be produced by the previous instructions: 

### 1. Standard JSON: 
The following two figures report the exectution time for standard JSON on (i) Desktop and (ii) Server.

<!-- ![Standard JSON - Desktop](fig/1-large-jupiter-1.png "Parsing Time of Standard JSON on Desktop")
![Standard JSON - Server](fig/1-large-HPCC-1.png "Parsing Time of Standard JSON on Server") -->

<figcaption style="text-align:center"><b>Fig.1 - Parsing Time of Standard JSON on Desktop.</b></figcaption>
<br/>
<img src="fig/1-large-jupiter-1.png" width="70%"></img>

<figcaption style="text-align:center"><b>Fig.2 - Parsing Time of Standard JSON on Server.</b></figcaption>
<br/>
<img src="fig/1-Large-HPCC-1.png" width="70%"></img>


### 2. JSON Lines: 
The following two figures report the exectution time for JSON Lines on (i) Desktop and (ii) Server.

<figcaption style="text-align:center"><b>Fig.3 - Parsing Time of JSON Lines on Desktop.</b></figcaption>
<br/>
<img src="fig/1-Small-Jupiter-1.png" width="70%"></img>

<figcaption style="text-align:center"><b>Fig.4 - Parsing Time of JSON Lines on Server.</b></figcaption>
<br/>
<img src="fig/1-Small-HPCC-1.png" width="70%"></img>


### 3. Breakdown
The following two figures report the breakdown time on (i) Desktop and (ii) Server.
<figcaption style="text-align:center"><b>Fig.3 - Costs of Five Stages on Desktop.</b></figcaption>
<br/>
<img src="fig/2-breakdown-jupiter-1.png" width="70%"></img>

<figcaption style="text-align:center"><b>Fig.4 - Costs of Five Stages on Server.</b></figcaption>
<br/>
<img src="fig/2-breakdown-hpcc-1.png" width="70%"></img>


<br>
<br>
<hr>


## Example and Query
We provide 2 examples for queries in the `./example` directory. You have to clone the whole project. Also, make sure to have the required prerequisites mentioned earlier.


### Example 1 (JSON Lines, Twitter):
1. compile the `./example/example1.cu`:
```
nvcc -O3 -o ./example1.out ./example/example1.cu -w -gencode=arch=compute_61,code=sm_61
```

**Note**: `-gencode=arch=compute_61,code=sm_61` will differ for different GPU architecture. 

2. run the `./example1.out`:
```
./example1.out -b ./datasets/twitter_sample_small_records.json
```
3. Expected output will be: 
```
Batch mode running...

Value: [query value]
Total Query time: [time for returning that query].

```


### Example 2 (Standard JSON, Twitter):
1. compile the `./test/example2.cu`:
```
nvcc -O3 -o ./example2.out ./test/example2.cu -w -gencode=arch=compute_61,code=sm_61
```

**Note**: `-gencode=arch=compute_61,code=sm_61` will differ for different GPU architecture. 

2. run the `./example2.out`:
```
./example2.out -b ./datasets/twitter_sample_large_record.json
```
3. Expected output will be: 
```
Batch mode running...

Value: [query value]
Total Query time: [time for returning that query].

```


**Note:** Download all datasets from [this link](https://drive.google.com/drive/folders/1PkDEy0zWOkVREfL7VuINI-m9wJe45P2Q?usp=sharing) and place them in the `dataset` folder. After that, use `twitter_small_records.json` instead of `twitter_sample_small_records.json` and `twitter_large_record.json` instead of `twitter_sample_large_record.json`.



## License

MIT
