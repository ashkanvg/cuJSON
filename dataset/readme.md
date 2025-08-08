## Datasets
Two sample datasets are included in `dataset` folder. Large datasets (used in performance evaluation) can be downloaded from https://drive.google.com/drive/folders/1PkDEy0zWOkVREfL7VuINI-m9wJe45P2Q?usp=sharing and placed into the `dataset` folder. 

- For JSON Lines, use those datasets that ended in `_small_records.json`. 
- For Standard JSON, use those datasets that ended in `_large_record.json`.


### Scalability 

The scalability folder is only for reproduced use of Scalability Result of our paper.


### MetaJSON
Since metaJSON will work only for branchless json files with pre-defined schema, we modify three of our datasets (Walmart, Bestbuy, Twitter) and store them into the `meta_json` directory from https://drive.google.com/drive/folders/1PkDEy0zWOkVREfL7VuINI-m9wJe45P2Q?usp=sharing link. 

It contatins: 
- 3 .JSON files
- 3 .cuh files

Per each dataset, you should replace the `data_def.cuh` with the requested dataset .cuh file (from the metaJSON files), later follow the running and compiling commands of meta-json repository (https://github.com/mis-wut/meta-json-parser).