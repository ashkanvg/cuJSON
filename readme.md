# cuJSON
cuJSON: Parsing Large JSON Data on a GPU

# Abstract
JSON (JavaScript Object Notation) has become a ubiquitous data format in modern computing, but its parsing can be a significant performance bottleneck. Contrary to the conventional wisdom that GPUs are not suitable for parsing due to the branching-rich nature of parsing algorithms, this work presents a novel GPU-based JSON parser, cuJSON, that redesigns the parsing algorithm to minimize branches and optimizes it for GPU architectures.

Inspired by recent advances in SIMD-based JSON processing, our work concentrates on exploiting bitwise parallelism, leveraging GPU intrinsic functions and high-performance CUDA libraries optimally. We introduce a novel output data structure that balances parsing and querying costs, and implement innovative techniques to break key dependencies in the parsing process. Through extensive experimentation, our evaluations demonstrate that cuJSON not only surpasses traditional CPU-based JSON parsers (like simdjson and Pison) but also outperforms existing GPU-based JSON parsers (such as cuDF and GPJSON), achieving unparalleled parsing speeds.


# Quick Start
The cuJSON library is easily consumable. 
0. Prerequisites: `g++` (version 7 or better), `Cuda` compilation tools (release 12.1), and a 64-bit system with a command-line shell (e.g., Linux, macOS, freeBSD). 
1. clone the `src` in your directoy. 
2. follow the following command to compile your the code:

```
nvcc -O3 -o [output.exe] [main.cu] -w [-gencode=arch=compute_61,code=sm_61]
```

3. download your JSON files and use this command line to parse it (default-version)
```
[output.exe] [JSON file] 
```
