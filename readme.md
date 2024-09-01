# cuJSON
cuJSON: Parsing Large JSON Data on a GPU

## Abstract
JSON (JavaScript Object Notation) has become a ubiquitous data format in modern computing, but its parsing can be a significant performance bottleneck. Contrary to the conventional wisdom that GPUs are not suitable for parsing due to the branching-rich nature of parsing algorithms, this work presents a novel GPU-based JSON parser, cuJSON, that redesigns the parsing algorithm to minimize branches and optimizes it for GPU architectures.

Inspired by recent advances in SIMD-based JSON processing, our work concentrates on exploiting bitwise parallelism, leveraging GPU intrinsic functions and high-performance CUDA libraries optimally. We introduce a novel output data structure that balances parsing and querying costs, and implement innovative techniques to break key dependencies in the parsing process. Through extensive experimentation, our evaluations demonstrate that cuJSON not only surpasses traditional CPU-based JSON parsers (like simdjson and Pison) but also outperforms existing GPU-based JSON parsers (such as cuDF and GPJSON), achieving unparalleled parsing speeds.


## Quick Start 
The cuJSON library is easily consumable. 
0. Prerequisites: `g++` (version 7 or better), `Cuda` compilation tools (release 12.1), and a 64-bit system with a command-line shell (e.g., Linux, macOS, freeBSD). 
1. clone the `src` in your directoy. 
2. follow the following command to compile your the code:

```
nvcc -O3 -o [output.exe] [./src/cuJSON.cu] -w [-gencode=arch=compute_61,code=sm_61]
```

3. download your JSON files and use this command line to parse it (default-version)
```
[output.exe] -b [JSON file] 
```
4. Your results is ready. It will print out the following results:
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

## datasets
Two sample datasets are included in `dataset` folder. Large datasets (used in performance evaluation) can be downloaded from https://drive.google.com/drive/folders/1KQ1DjvIWpHikOg1JgmjlSWM3aAlvq-h7?usp=sharing and placed into the `dataset` folder. 

## Example and Query
We provide 2 different examples in `./test` directory. For all of the examples, you have to clone whole project. Also, make sure to have the required prerequisites that mentioned earlier.


### Example 1 (JSON Lines, Twitter):
1. compile the `./test/example1.cu`:
```
nvcc -O3 -o ./example1.out ./test/example1.cu -w [-gencode=arch=compute_61,code=sm_61]
```

Note: `-gencode=arch=compute_61,code=sm_61` will be different for different GPU architecture. 

2. run the `./example1.out`:
```
./example1.out -b ./datasets/twitter_small_records.json
```

Note: for running Example 2 (Standard JSON, Twitter), you have tow change the `./test/example1.cu` to `./test/example2.cu` and also `./datasets/twitter_small_records.json` to `./datasets/twitter_large_record.json`.


## License

MIT
