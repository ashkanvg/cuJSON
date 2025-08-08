#!/bin/bash

# Run all figure scripts sequentially
./setup/clean.sh
./setup/setup_cudf_env.sh
./setip/install_gpjson.sh


echo "✅ Setup Successfully!"
