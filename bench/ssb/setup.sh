#!/usr/bin/env bash
set -euo pipefail

SF=${1:-1}
PATCH_FILE="../../../../bench/ssb/ssb.patch"

if [[ "$OSTYPE" == "darwin"* ]]; then
  MACHINE="MACOS"
  CORES=$(sysctl -n hw.ncpu)
  GNUSED="gsed"
else
  MACHINE="LINUX"
  CORES=$(nproc)
  GNUSED="sed"
fi

echo "Generating Star Schema Benchmark database with scale factor $SF"

mkdir -p "bench/data/ssb/sf$SF"
cd "bench/data/ssb"

# Originally from: https://www.cs.umb.edu/%7Eponeil/dbgen.zip
echo '284118b10f066fd0d5b8e4e77b150d0b6176510f84a2f54f93c3a439ac312bc3  dbgen.zip' | sha256sum --check --status 2>/dev/null || curl -OL https://db.in.tum.de/~fent/dbgen/ssb/dbgen.zip
echo '284118b10f066fd0d5b8e4e77b150d0b6176510f84a2f54f93c3a439ac312bc3  dbgen.zip' | sha256sum --check --status
unzip -q -u dbgen.zip

# Reuse existing datasets
if [ -z "$(ls -A "sf$SF")" ]; then
  (
    cd dbgen
    rm -rf ./*.tbl

    # Only apply patch if it hasn't been applied yet
    if patch -p5 --dry-run < ${PATCH_FILE} >/dev/null 2>&1; then
        patch -p5 < ${PATCH_FILE}
    else
        echo "Patch already applied or not needed, skipping."
    fi


    $GNUSED -i 's/#define  MAXAGG_LEN    10/#define  MAXAGG_LEN    20/' shared.h
    $GNUSED -i 's/|O_LARGEFILE//' bm_utils.c
    # fix include
    $GNUSED -i 's/<malloc.h>/<stdlib.h>/' bm_utils.c
    MACHINE="$MACHINE" make -sj "$CORES" dbgen 2>/dev/null
    ./dbgen -f -T c -s "$SF"
    ./dbgen -qf -T d -s "$SF"
    ./dbgen -qf -T p -s "$SF"
    ./dbgen -qf -T s -s "$SF"
    ./dbgen -q -T l -s "$SF"

    for table in ./*.tbl; do
      $GNUSED 's/|$//' "$table" >"../sf$SF/$table"
    done
  )
fi
echo "done"
