# rapidJSON
Here are instructions on running rapidJSON for Standandard JSON.

## Prerequisites: 
- `g++` (version 7 or better), 
- and a 64-bit system with a command-line shell (e.g., Linux, macOS, FreeBSD). 


## Quick Start
We put two different possible codes. 
1. Only report the parsing time
2. The performance of each query mentioned in the paper.  

### 1. Parsing (MAKEFILE)
1. Make sure to clone our GitHub repository (all of the mentioned passes are from the leading directory of the repository)
2. Download All Dataset: download all datasets from https://drive.google.com/drive/folders/1PkDEy0zWOkVREfL7VuINI-m9wJe45P2Q?usp=sharing and place them into the `dataset` folder.

3. Compile all files: Run the following command to compile all six datasets files:

```
Make
```

Note: If you want to run only one or two datasets instead of all of them, modify the `./related_works/rapidjson/Makefile`. You need to comment on the function call for non-wanted datasets.


4. Run all executables: To run all executables with a message before each one, use:
```
make run
```

5. Clean all executables: To remove all compiled .exe files, run:
```
make clean
```

<!-- 
### 2. query results
1. Make sure to clone our GitHub repository (all of the mentioned passes are from the leading directory of the repository)
2. Download All Dataset: download all datasets from https://drive.google.com/drive/folders/1PkDEy0zWOkVREfL7VuINI-m9wJe45P2Q?usp=sharing and place them into the `dataset` folder.

Note: If you want to run only one or two datasets instead of all of them, modify the `quickstart-original-iterate-query.cpp`. You need to comment on the function call for non-wanted datasets.

3. Compile: 

```
g++ ./related_works/simdjson/SIMDJSON.cpp ./related_works/simdjson/quickstart-original-iterate.cpp -O3 -o out-query.exe -std=c++17
```

4. Run: 
```
./out-query.exe
``` -->



## More Details and References
More information is available at https://github.com/simdjson/simdjson.

- Geoff Langdale, Daniel Lemire, [Parsing Gigabytes of JSON per Second](https://arxiv.org/abs/1902.08318), VLDB Journal 28 (6), 2019.
- John Keiser, Daniel Lemire, [Validating UTF-8 In Less Than One Instruction Per Byte](https://arxiv.org/abs/2010.03090), Software: Practice & Experience 51 (5), 2021.
