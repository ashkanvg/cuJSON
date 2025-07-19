
# Reproduced CSV of the Results

All runs were executed 10 times to calculate the average results for each method.

## 📊 Fig 9 and Fig 10 (cuJSON vs CPU Methods)

To run each method separately and get the results for each dataset (executed 10 times), use the following commands:

- **cuJSON**  
  ```bash
  ./run_cujson_fig9.sh
  ```

- **simdJSON**  
  ```bash
  ./run_simdjson_fig9.sh
  ```

- **RapidJSON**  
  ```bash
  ./run_rapidjson_fig9.sh
  ```

- **Pison**  
  ```bash
  ./run_pison_fig9.sh
  ```

If you'd like to run all methods together, simply execute:

```bash
run_all_fig9.sh
```

### Results Location:
- All method results can be found in `scripts/results/fig9_data.csv`.
- Individual method results are saved in `scripts/results/` under:
  - `cujson_fig9.csv`
  - `simdjson_fig9.csv`
  - `rapidjson_fig9.csv`
  - `pison_fig9.csv`

---

## 📈 Fig 11 (cuJSON vs GPU Methods)

To run each method separately for Fig 11:

- **GPJSON**  
  ```bash
  ./run_gpjson_fig11.sh
  ```

- **cuDF**  
  ```bash
  ./run_cuDF_fig11.sh
  ```

If you'd like to run both methods together, use the following:

```bash
run_all_fig11.sh
```

### Results Location:
- All method results are stored in `scripts/results/fig11_data.csv`.
- Individual results can be found in `scripts/results/` under:
  - `cujson_fig9.csv`
  - `gpjson_fig11.csv`
  - `cudf_fig11.csv`

---

## 💻 Fig 12: Peak GPU Memory

To report peak GPU memory usage for each method:

- **cuJSON**  
  ```bash
  ./run_cujson_fig12.sh
  ```

- **GPJSON**  
  ```bash
  ./run_gpjson_fig12.sh
  ```

- **cuDF**  
  ```bash
  ./run_cudf_fig12.sh
  ```

To run all methods together:

```bash
./run_all_fig12.sh
```

> **Note**: MetaJSON is not included in performance tests as it requires very huge compile time and only works for a few simplified test cases without branches. We include it only to show that our proposed method outperforms it.

### Results Location:
- All method results are stored in `scripts/results/fig12_data.csv`.
- Individual results can be found in `scripts/results/` under:
  - `cujson_fig12.csv`
  - `gpjson_fig12.csv`
  - `cudf_fig12.csv`

---

## ⏱️ Fig 13 and Table 8 (cuJSON only)

For time-breakdown reporting with **cuJSON**, execute the following:

```bash
./run_cujson_fig13.sh
```

> **Note**: We are unable to provide scripts for other methods' time-breakdowns due to the need for code modifications after compilation and library installation. 

### Results Location:
- All method results are stored in `scripts/results/fig13_data.csv`.



---

## 🧠 Fig 14: Reporting Output Memory

To run each method separately and report memory usage:

- **cuJSON**  
  ```bash
  ./run_cujson_fig14.sh
  ```

- **simdJSON**  
  ```bash
  ./run_simdjson_fig14.sh
  ```

- **RapidJSON**  
  ```bash
  ./run_rapidjson_fig14.sh
  ```

- **Pison**  
  ```bash
  ./run_pison_fig14.sh
  ```

- **cuDF/MetaJSON**  
  ```bash
  ./run_cudf_fig14.sh
  ```

> **Note**: To compute results for **GPJSON**, the library requires modifications. This library lead to no output and it required to modify the source code after installation.

To run all methods together:

```bash
./run_all_fig14.sh
```

### Results Location:
- All method results are stored in `scripts/results/fig14_data.csv`.
- Individual results can be found in `scripts/results/` under:
  - `cujson_fig14.csv`
  - `simdjson_fig14.csv`
  - `rapidjson_fig14.csv`
  - `pison_fig14.csv`
  - `cudf_fig14.csv`

---

## 🕒 Fig 15: Running Query Timings

### Left: Average Query Time

To report the average time for running the queries, execute the scripts for each method:

- **cuJSON**  
  ```bash
  ./run_cujson_fig15.sh
  ```

- **simdjson**  
  ```bash
  ./run_simdjson_fig15.sh
  ```

- **Pison**  
  ```bash
  ./run_pison_fig15.sh
  ```

- **RapidJSON**  
  ```bash
  ./run_rapidjson_fig15.sh
  ```

> **Note**: The results will include the average time displayed in the final row of the output.

To run all methods together:

```bash
./run_all_fig15.sh
```

### Results Location:
- All method results are stored in `scripts/results/fig15_data.csv`.
- Individual results can be found in `scripts/results/` under:
  - `cujson_fig15.csv`
  - `simdjson_fig15.csv`
  - `rapidjson_fig15.csv`
  - `pison_fig15.csv`

---

### Middle and Right: Modifying JSON Files

For these sections, you will need to modify the JSON file to contain only one record to compute the query time. Library modifications are required for **GPJSON** and **cuDF**, so ensure you install the libraries and use them accordingly.


---

## 🧑‍💻 Fig 16: cuJSON Scalability

To run **cuJSON** scalability tests:

1. First, download the scalability data.
2. Then, execute the following:

```bash
./run_cujson_fig16.sh
```

### Results Location:
- All method results are stored in `scripts/results/fig16_data.csv`.


---

## General Notes:

- All scripts assume you have the necessary dependencies installed.
- For `GPJSON` after you install the required library files, you have to add your keys in the scripts: `scripts/run_gpjson_fig11.sh` and `scripts/run_gpjson_fig12.sh`.
- The `scripts/results` folder will contain all output files, categorized by method and csvs.
- For specific modifications or troubleshooting, refer to the individual script files, or readme of each relarted works for more details.
- Make sure to download all the datasets can be downloaded from https://drive.google.com/drive/folders/1PkDEy0zWOkVREfL7VuINI-m9wJe45P2Q?usp=sharing and placed into the `dataset` folder. `scabality` folder must place exactly like what it is in the `dataset` folder for proper experiment.


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


