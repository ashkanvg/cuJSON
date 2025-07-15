# rapidJSON
Here are instructions on running pison for Standandard JSON and JSON Lines.

## Prerequisites: 
- `g++` (version 7 or better), 
- and a 64-bit system with a command-line shell (e.g., Linux, macOS, FreeBSD). 


## Quick Start
We put two different possible codes. 
1. Standard JSON
2. JSON Lines
<!-- 2. The performance of each query mentioned in the paper.   -->

### 1. Parsing (Standard JSON)
1. Make sure to clone our GitHub repository (all of the mentioned passes are from the leading directory of the repository)
2. Download All Dataset: download all datasets from https://drive.google.com/drive/folders/1PkDEy0zWOkVREfL7VuINI-m9wJe45P2Q?usp=sharing and place them into the `dataset` folder.
3. Move to pison directory
```
cd ./related_works/pison/standard_JSON
```

4. Compile all files: Run the following command to compile all six datasets files (for 8 thread):

```
Make all
```

Note: If you want to run only one or two datasets instead of all of them, modify the `./related_works/pison/standard_JSON/Makefile`. You need to comment on the function call for non-wanted datasets.


5. Run all executables: To run all executables with a message before each one, use:
```
cd bin; ./twitter; ./bestbuy; ./google; ./nspl; ./walmart; ./wiki; 
```

6. Clean all executables: To remove all compiled .exe files, run:
```
cd ..
make clean
```


### 2. Parsing (JSON Lines)
1. Make sure to clone our GitHub repository (all of the mentioned passes are from the leading directory of the repository)
2. Download All Dataset: download all datasets from https://drive.google.com/drive/folders/1PkDEy0zWOkVREfL7VuINI-m9wJe45P2Q?usp=sharing and place them into the `dataset` folder.
3. Move to pison directory
```
cd ./related_works/pison/JSON_lines
```

4. Compile all files: Run the following command to compile all six datasets files (for 8 thread):

```
Make all
```

Note: If you want to run only one or two datasets instead of all of them, modify the `./related_works/pison/JSON_lines/Makefile`. You need to comment on the function call for non-wanted datasets.


5. Run all executables: To run all executables with a message before each one, use:
```
cd bin; ./bestbuy; ./google; ./nspl; ./twitter; ./walmart; ./wiki; 
```

6. Clean all executables: To remove all compiled .exe files, run:
```
cd ..
make clean
```


## More Details and References
More information is available at https://github.com/AutomataLab/Pison/tree/master

- Lin Jiang, Junqiao Qiu, Zhijia Zhao. Scalable Structural Index Construction for JSON Analytics. PVLDB, 14(4):694-707, 2021.
- Yinan Li, Nikos R. Katsipoulakis, Badrish Chandramouli, Jonathan Goldstein, D. Kossmann. Mison: A Fast JSON Parser for Data Analytics. PVLDB, 10(10): 2017.
- Langdale, Geoff, and Daniel Lemire. "Parsing gigabytes of JSON per second." The VLDB Journal 28, no. 6 (2019): 941-960.