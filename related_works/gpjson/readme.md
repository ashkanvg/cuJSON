# rapidJSON
Here are instructions on running GPJSON for JSON Lines.

## Prerequisites: 
- `g++` (version 7 or better), 
- and a 64-bit system with a command-line shell (e.g., Linux, macOS, FreeBSD). 
- GPJSON uses Gradle as a build system. 
- The only requirements for running GPJSON are `GraalVM` and `CUDA` (and a supported GPU). It has been tested on `GraalVM CE 21.0.0.2 Java 8`, downloadable here (https://github.com/graalvm/graalvm-ce-builds/releases/tag/vm-21.0.0.2). 
- It has only been tested on Linux using `CUDA 11.2`, but should work on other OS when `CUDARuntime.java` is adapted.

## Parser
1. Make sure to clone our GitHub repository (all of the mentioned passes are from the leading directory of the repository)
2. Download All Dataset: download all datasets from https://drive.google.com/drive/folders/1PkDEy0zWOkVREfL7VuINI-m9wJe45P2Q?usp=sharing and place them into the `dataset` folder.
3. Move to gpjson directory
```
cd ./related_works/gpjson
```

4. build and run:
All of the information of how to install the prerequisites are mentioned in their github: https://github.com/koesie10/gpjson

5. run: You only need to run gpjson.js after installing prerequisites.
```
$GRAALVM_HOME/bin/node --polyglot --jvm gpjson-bestbuy.js
$GRAALVM_HOME/bin/node --polyglot --jvm gpjson-google.js
$GRAALVM_HOME/bin/node --polyglot --jvm gpjson-nspl.js
$GRAALVM_HOME/bin/node --polyglot --jvm gpjson-twitter.js
$GRAALVM_HOME/bin/node --polyglot --jvm gpjson-walmart.js
$GRAALVM_HOME/bin/node --polyglot --jvm gpjson-wiki.js
```


