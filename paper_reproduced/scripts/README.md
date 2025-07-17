# Reproduced CSV of the Results:
All of the runs are average of 10 times of repearitng the exectuion. 

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


# Fig 11:
- GPJSON 
```
./run_gpjson_fig11.sh
```
- cuDF
```
./run_cuDF_fig11.sh
```

In order to run all the resutls together:
```
run_all_fig11.sh
```

You can find the results of all methods: `scripts/results/fig9_data.csv` <br>
You can find the resutls of each methods in `scripts/results`: `cujson_fig9_temp.sh`, `simdjson_fig9_tmp.sh`, `rapidjson_fig9_tmp.sh`, and `pison_fig9_tmp.sh`.


## Fig 12: 
Peak GPU Memory: 
- cuJSON
```
./run_cujson_fig12.sh
```
- GPJSON
```
./run_gpjson_fig12.sh
```
- cuDF
```
./run_cujson_fig12.sh
```

In order to run all the results together: 
```
./run_all_fig12.sh
```
<!-- MetaJSON having long compile time and we skip the performance because it is not related at all to our proposed method. As we mentioned in the paper, It only works for a few modified test cases without any branches. We only cover it to show that our paper is also outperforms it. -->

## Fig 13 and Table 8 (only cuJSON):
In order to report time-breakdowns:
```
./run_cujson_fig13.sh
```
<!-- We are not able to provide the script for the time-breakdown for the other methods, because it requires to change the library after compilation by changing the inside of the code.  -->





## Fig 15 (only cuJSON):
In order to report the average time of running the queries, please run the mentioned script:
```
./run_cujson_fig15.sh
```

It reports the value of the each query, and average of of all of them in final row of the output.

