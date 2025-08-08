
# Reproduced CSV of the Results

All runs were executed 10 times to calculate the average results for each method.

## Datasets
Two sample datasets are included in the `dataset` folder. Large datasets (used in performance evaluation) can be downloaded from https://drive.google.com/drive/folders/1PkDEy0zWOkVREfL7VuINI-m9wJe45P2Q?usp=sharing and placed into the `dataset` folder. 

- For JSON Lines, use those datasets that end in `_small_records.json`. 
- For Standard JSON, use those datasets that end in `_large_record.json`.
 
## Setup:
**RapidJSON**, **simdjson**, **cuJSON**, and **pison** are already available in the related_works directory. No need for further installation. For **cudf** and **GPJSON**, it requires to install their library and create their enviroment to be used which are also explained in the `related_works/gpjson` and `related_works/cudf`'s `readme.md` section. 

We provide a script to setup gpjson and cudf too. You can use:
```
./setup.sh
```

> **Note**: It contains three scripts, `./setup/install_gpjson.sh` will download GraalVM (a dependency for gpjson) as well as clone and build gpjson. It's output also advises on what environment variables need to be set. `./setup/setup_cudf_env.sh` will setup cudf within a conda environment following steps from their documentation (https://github.com/rapidsai/cudf). `./setup/clean.sh` will just clean up the installation of gpjson, graalvm, and remove the cudf_env conda environment.

> **Note**: MetaJSON is not included in performance tests as it requires very huge compile time and only works for a few simplified test cases without branches. We include it only to show that our proposed method outperforms it. MetaJSON discusses how to parse JSON objects with a fixed JSON schema, which greatly simplifies the task of data de-serialization. However, if you want to test meta-json, you can follow their github repository: https://github.com/mis-wut/meta-json-parser . For datasets, please refer to our `dataset` folder and its corresponding `readme` since we have specific json files the preprocessed for `meta-json`. 
Per each dataset, you should manually replace the `data_def.cuh` (from the metaJSON files) with the requested dataset .cuh file (from google drive), later follow the running and compiling commands of meta-json repository (https://github.com/mis-wut/meta-json-parser).


## 📊 Run all experiments
We have added a new script called `run_experiments.sh`, which executes all experiments and generates the final figures and tables presented in the paper. For other methods, their libraries are already included in the `related_works` folder. If you need for further manually scritps checking and runnign per each figure you can follow the following subsections.


### 📊 Fig 9 and Fig 10 (cujson vs CPU Methods)

To run each method separately and get the results for each dataset (executed 10 times), use the following commands:

- **cujson**  
For running the cuJSON make sure to edit the `-gencode=arch=compute_61,code=sm_61` in the compile line of the scripts based on your GPU compatibilty version.
  ```bash
  ./run_cujson_fig9.sh
  ```

- **simdjson**  
  ```bash
  ./run_simdjson_fig9.sh
  ```

- **rapidjson**  
  ```bash
  ./run_rapidjson_fig9.sh
  ```

- **pison**  
  ```bash
  ./run_pison_fig9.sh
  ```

If you'd like to run all methods together, simply execute:

```bash
run_all_fig9.sh
```

#### Results Location:
- All method results can be found in `scripts/results/fig9_data.csv`.
- Individual method results are saved in `scripts/results/` under:
  - `cujson_fig9.csv`
  - `simdjson_fig9.csv`
  - `rapidjson_fig9.csv`
  - `pison_fig9.csv`

---

### 📈 Fig 11 (cujson vs GPU Methods)

To run each method separately for Fig 11:
- **cujson**  
For running the cuJSON make sure to edit the `-gencode=arch=compute_61,code=sm_61` in the compile line of the scripts based on your GPU compatibilty version.
  ```bash
  ./run_cujson_fig9.sh
  ```

- **gpjson**  
  ```bash
  ./run_gpjson_fig11.sh
  ```

- **cudf**  
  ```bash
  ./run_cudf_fig11.sh
  ```

If you'd like to run both methods together, use the following:

```bash
run_all_fig11.sh
```

> **Note**: MetaJSON is not included in performance tests as it requires very huge compile time and only works for a few simplified test cases without branches. We include it only to show that our proposed method outperforms it. MetaJSON discusses how to parse JSON objects with a fixed JSON schema, which greatly simplifies the task of data de-serialization. However, if you want to test meta-json, you can follow their github repository: https://github.com/mis-wut/meta-json-parser . For datasets, please refer to our `dataset` folder and its corresponding `readme` since we have specific json files the preprocessed for `meta-json`.


#### Results Location:
- All method results are stored in `scripts/results/fig11_data.csv`.
- Individual results can be found in `scripts/results/` under:
  - `cujson_fig9.csv`
  - `gpjson_fig11.csv`
  - `cudf_fig11.csv`

---

### 💻 Fig 12: Peak GPU Memory

To report peak GPU memory usage for each method:

- **cujson**  
For running the cuJSON make sure to edit the `-gencode=arch=compute_61,code=sm_61` in the compile line of the scripts based on your GPU compatibilty version.
  ```bash
  ./run_cujson_fig12.sh
  ```

- **gpjson**  
  ```bash
  ./run_gpjson_fig12.sh
  ```

- **cudf**  
  ```bash
  ./run_cudf_fig12.sh
  ```

To run all methods together:

```bash
./run_all_fig12.sh
```

> **Note**: MetaJSON is not included in performance tests as it requires very huge compile time and only works for a few simplified test cases without branches. We include it only to show that our proposed method outperforms it. MetaJSON discusses how to parse JSON objects with a fixed JSON schema, which greatly simplifies the task of data de-serialization. However, if you want to test meta-json, you can follow their github repository: https://github.com/mis-wut/meta-json-parser . For datasets, please refer to our `dataset` folder and its corresponding `readme` since we have specific json files the preprocessed for `meta-json`.

#### Results Location:
- All method results are stored in `scripts/results/fig12_data.csv`.
- Individual results can be found in `scripts/results/` under:
  - `cujson_fig12.csv`
  - `gpjson_fig12.csv`
  - `cudf_fig12.csv`

---

### ⏱️ Fig 13 and Table 8 (cujson only)

For time-breakdown reporting with **cujson**, execute the following:

For running the cuJSON make sure to edit the `-gencode=arch=compute_61,code=sm_61` in the compile line of the scripts based on your GPU compatibilty version.

```bash
./run_cujson_fig13.sh
```

> **Note**: We are unable to provide scripts for other methods' time-breakdowns due to the need for code modifications after compilation and library installation. 

#### Results Location:
- All method results are stored in `scripts/results/fig13_data.csv`.



---

### 🧠 Fig 14: Reporting Output Memory

To run each method separately and report memory usage:

- **cujson**  
For running the cuJSON make sure to edit the `-gencode=arch=compute_61,code=sm_61` in the compile line of the scripts based on your GPU compatibilty version.
  ```bash
  ./run_cujson_fig14.sh
  ```

- **simdjson**  
  ```bash
  ./run_simdjson_fig14.sh
  ```

- **rapidjson**  
  ```bash
  ./run_rapidjson_fig14.sh
  ```

- **pison**  
  ```bash
  ./run_pison_fig14.sh
  ```

- **cudf/MetaJSON**  
  ```bash
  ./run_cudf_fig14.sh
  ```

> **Note**: To compute results for **gpjson**, the library requires modifications. This library lead to no output and it required to modify the source code after installation. However, Theoritcally, the best possible results of `gpjson` will be as same as `pison`, but by modification we realize it is even worst than `pison`.

To run all methods together:

```bash
./run_all_fig14.sh
```

#### Results Location:
- All method results are stored in `scripts/results/fig14_data.csv`.
- Individual results can be found in `scripts/results/` under:
  - `cujson_fig14.csv`
  - `simdjson_fig14.csv`
  - `rapidjson_fig14.csv`
  - `pison_fig14.csv`
  - `cudf_fig14.csv`

---

### 🕒 Fig 15: Running Query Timings

#### Left: Average Query Time

To report the average time for running the queries, execute the scripts for each method:

- **cujson**  
For running the cuJSON make sure to edit the `-gencode=arch=compute_61,code=sm_61` in the compile line of the scripts based on your GPU compatibilty version.

  ```bash
  ./run_cujson_fig15.sh
  ```

- **simdjson**  
  ```bash
  ./run_simdjson_fig15.sh
  ```

- **pison**  
  ```bash
  ./run_pison_fig15.sh
  ```

- **rapidjson**  
  ```bash
  ./run_rapidjson_fig15.sh
  ```

> **Note**: The results will include the average time displayed in the final row of the output.

To run all methods together:

```bash
./run_all_fig15.sh
```

#### Results Location:
- All method results are stored in `scripts/results/fig15_data.csv`.
- Individual results can be found in `scripts/results/` under:
  - `cujson_fig15.csv`
  - `simdjson_fig15.csv`
  - `rapidjson_fig15.csv`
  - `pison_fig15.csv`

---

#### Middle and Right: Modifying JSON Files

For these sections, you will need to modify the JSON file to contain only one record to compute the query time. Library modifications are required for **gpjson** and **cudf**, so ensure you install the libraries and use them accordingly.


---

### 🧑‍💻 Fig 16: cujson Scalability

To run **cujson** scalability tests:

1. First, download the scalability data.
2. Then, execute the following:

```bash
./run_cujson_fig16.sh
```
For running the cuJSON make sure to edit the `-gencode=arch=compute_61,code=sm_61` in the compile line of the scripts based on your GPU compatibilty version.

#### Results Location:
- All method results are stored in `scripts/results/fig16_data.csv`.


---

## General Notes:

> **Note 1:** All scripts assume you have the necessary dependencies installed.

> **Note 2:** For `gpjson` after you install the required library files, you have to add your keys in the scripts: `scripts/run_gpjson_fig11.sh` and `scripts/run_gpjson_fig12.sh`.

> **Note 3:** The `scripts/results` folder will contain all output files, categorized by method and csvs.

> **Note 4:** For specific modifications or troubleshooting, refer to the individual script files, or readme of each relarted works for more details.

> **Note 5:** Make sure to download all the datasets can be downloaded from https://drive.google.com/drive/folders/1PkDEy0zWOkVREfL7VuINI-m9wJe45P2Q?usp=sharing and placed into the `dataset` folder. `scabality` folder must place exactly like what it is in the `dataset` folder for proper experiment.

> **Note 6:** For running the cuJSON make sure to edit the `-gencode=arch=compute_61,code=sm_61` in the compile line of the scripts based on your GPU compatibilty version.


---

## Figures Generator
We also provide a script that will use for generate figures of the paper. 
If you'd like to run generate the figure, simply execute. After you generate the csv files, you can run this srcip:

```bash
figure_generator.sh
```

> **Note**: You can select which figures you want to generate by modifying `figure_generator.sh` and comments the figures that you do not want to generate.



### Results Location:
- All figures are stored `scripts/figures/`.




Happy experimenting! 🚀


