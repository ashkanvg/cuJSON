# rapidJSON
Here are instructions on running cuDF for JSON Lines.

## Prerequisites: 
- `python` (version 3.1 or better), 
- `gcc` version 11.4+
- `nvcc` version 11.8+
- `cmake` version 3.29.6+
- `CUDA` version 11.4+
- Volta architecture or better (Compute Capability >=7.0)


## Parser
1. Make sure to clone our GitHub repository (all of the mentioned passes are from the leading directory of the repository)
2. Download All Dataset: download all datasets from https://drive.google.com/drive/folders/1PkDEy0zWOkVREfL7VuINI-m9wJe45P2Q?usp=sharing and place them into the `dataset` folder.
3. Move to cuDF directory
```
cd ./related_works/cuDF
```

4. build and run:
All of the information of how to install the prerequisites are mentioned in their github and documents: 
- https://docs.rapids.ai/api/cudf/legacy/user_guide/io/read-json/
- https://github.com/rapidsai/cudf
- https://github.com/rapidsai/cudf/blob/branch-24.12/CONTRIBUTING.md#setting-up-your-build-environment

5. run: You only need to run python file after installing prerequisites.
```
python ./bestbuy.py;
python ./google.py;
python ./nspl.py;
python ./twitter.py;
python ./walmart.py;
python ./wiki.py;
```


