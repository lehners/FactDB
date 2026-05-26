#!/bin/bash

echo "Setup Umbra LE..."
./scripts/bench/setup_umbra_le.sh

echo "Load systems..."
./scripts/bench/load_fdb.sh
./scripts/bench/load_lmfao.sh

echo "Load data..."
./bench/ce/setup.sh
./bench/ssb/setup.sh 1
./bench/ssb/setup.sh 10

# create files for microbenchmark...
echo "Generate artificial data"
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j${nproc} benchArtificial
./benchArtificial -mGraphviz -talphaZipf -agk

echo "Download estimates.db - this might take a while"
curl -fSL https://db.in.tum.de/~lehner/estimates.db -o factDB/gen/estimates.db