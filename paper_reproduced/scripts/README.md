# Reproduced CSV of the Results:
## Fig 9 and Fig 10:

In order to run each method seperately, the results is per each dataset and 10 times of repeating the execution: 
- cuJSON
```
./run_cujson_fig9.sh
```
- simdJSON
```
./run_cujson_fig9.sh
```
- RapidJSON
```
./run_rapidjson_fig9.sh
```
- Pison 
```
./run_pison_fig9.sh
```

In order to run all the resutls together:
```
run_all_fig9.sh
```

You can find the results of all methods: `scripts/results/fig9_data.csv` <br>
You can find the resutls of each methods in `scripts/results`: `cujson_fig9_temp.sh`, `simdjson_fig9_tmp.sh`, `rapidjson_fig9_tmp.sh`, and `pison_fig9_tmp.sh`.


<!-- # Fig 11:

# Fig 12: -->


## Fig 13, Table 8 (only cuJSON):
In order to report time-breakdowns:
```
./run_cujson_fig13.sh
```


## Fig 15 (only cuJSON):
In order to report the average time of running the queries, please run the mentioned script:
```
./run_cujson_fig15.sh
```

It reports the value of the each query, and average of of all of them in final row of the output.

