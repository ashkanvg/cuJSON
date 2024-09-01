# GJSON
GJSON: Parsing Large JSON Data on a GPU

# Abstract
Semi-structured data, like JSON, are fundamental to the Web and document data stores. A common way to access large JSON files is by creating a parsed tree and then requesting corresponding keys and values. But, due to the increasing size of JSON files, CPU-based JSON parsers are becoming inefficient and time-consuming.

To overcome this issue, GJSON has been developed as the first GPU-based framework for parsing large JSON files. However, there are many dependencies among JSON files when we try to divide them into smaller byte sizes. To address this, a highly bit-parallel solution that uses bitwise and GPU operations has been designed to identify dependencies and irrelevant substructures during parsing. GJSON has three highly parallel parts: validation, tokenization, and parser. It also offers a set of APIs that can be used for generating parsed trees and returning query path requests.

Real-world datasets have been evaluated, and the results show that GJSON can achieve significant speedups over the state-of-the-art JSON parsers, especially for JSON documents with many small JSON records.

# Quick Start
The GJSON library is easily consumable. 
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
