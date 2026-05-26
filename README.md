# FactDB - a relational factorized query engine

FactDB is a high-performance relational database engine that focuses on query execution.
It implements a traditional, flat query engine with table scans, hash joins, and count(*) aggregations.
It supports also normal printing of the results.

The flat engine is extended to a **factorized execution engine** that supports with the same capabilities.
It is described more in detail in the paper: "The Data World Is Not Flat: Efficient Factorized Execution for Relational Systems"

## Overview

FactDB is built in C++23 and provides:

- **SQL Query Support**: SQL parsing and execution capabilities
- **Factorized Computation**: Factorized query execution for complex join operations
- **Code Generation**: JIT-style compilation for query performance optimization
- **Benchmarking Suite**: Comprehensive benchmarking tools for performance analysis and evaluation
- **Multi-threaded Execution**: Parallel query execution using Intel TBB

## Key Components

The rough structure of the project is as follows:

- `bench/` **Benchmarking**: Tools for running and analyzing benchmarks (SSB, CE, microbenchmarks)
- `factDB/` **Database Core**: Main database engine with schema management and query execution
- `infra/` **Infrastructure**: Core utilities including allocators, data generators, and configuration
- `factDB/algebra/` **Algebra**: Query algebra operators (joins, selections, scans, etc.)
- `factDB/gen/` **Generated Query files**: Generated files for query execution
- `factDB/infra/` **Infrastructure**: More infrastructure code related to the database engine
- `factDB/newftree/` **F-Tree code generation**: Logic to generate code for f-representations
- `factDB/opt/` **Query Optimizer**: Query optimization (currently deactivated)
- `factDB/parser/` **Parser**: SQL parsing using Bison/Flex
- `factDB/queryc/` **Query Compilation**: Query code compilation utils
- `factDB/schemac/` **Schema Compilation**: Code to generate code for table schemas
- `factDB/statement/` **SQL Statement Handler**: Representations and handling of SQL statements
- `factDB/util/` **Utility functionality**: Utility functions and classes used across the project

For the paper "The Data World Is Not Flat", the following files implement most of the described functionality:
- [`factDB/newftree/FTree.cpp`](factDB/newftree/FTree.cpp): Models f-trees internal and provide the interface to merge two f-trees. The generation of **root to leaf paths** can be found therein.
- [`factDB/infra/ChainingHashTable.hpp`](factDB/infra/ChainingHashTable.hpp): Implementation of a chaining hash table used for joins and aggregations.
- [`factDB/algebra/visitors/CodegenExpanded.cpp`](factDB/algebra/visitors/CodegenExpanded.cpp): Generates the flat query code.
- [`factDB/algebra/visitors/CodegenFactorized.cpp`](factDB/algebra/visitors/CodegenFactorized.cpp): Generates the factorized query code.

These files implement the benchmark and the corresponding data generation for the micro benchmarks.
- [`bench/bench.cpp`](bench/bench.cpp): Contains the benchmark suite for the CE and SSB evaluation in the paper.
- [`bench/artificial/bench.cpp`](bench/artificial/bench.cpp): Contains the benchmark suite for the microbenchmark evaluation in the paper and calls the data generators.
- [`bench/artificial/DataGen.cpp`](bench/artificial/DataGen.cpp): Contains the data generators.

We provide the generated code for query dblp_acyclic_201_00 for our flat and factorized engines  [`factDB/gen/query`](factDB/gen/query).
- [`query_CodegenFactorized_picb_dblp_acyclic_201_00.cpp`](factDB/gen/query/query_CodegenFactorized_picb_dblp_acyclic_201_00.cpp): Factorized code, which also contains the generated implementations of the f-representations.
- [`query_CodegenFlat_picb_dblp_acyclic_201_00.cpp`](factDB/gen/query/query_CodegenFlat_picb_dblp_acyclic_201_00.cpp): Flat code without code for containers, since it relies on templates.
- [`query_CodegenFactorized_picb_WINF.cpp`](factDB/gen/query/query_CodegenFactorized_picb_WINF.cpp): Factorized code for the example query from the paper (cf. Figure 4).

## Installation

### Prerequisites

Install required system dependencies:

```shell
sudo apt-get install libgtest-dev libfmt-dev libtbb-dev libpq-dev flex bison libre2-dev libsqlite3-dev
```

### Building with CMake

```shell
mkdir cmake-build-release
cd cmake-build-release
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
```

For debug builds:

```shell
mkdir cmake-build-debug
cd cmake-build-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build .
```

### Setup
For a quickstart, we provide the script `./scripts/bench/setup`. 
This script does the following:
1. Load the Umbra LE Docker Image as given by their reproducability package (requires an active installation of docker)
2. Load FBench and LMFAO
3. Load the data for the CE benchmark and the SSB (both, scale factor 1 and 10)
4. Generates the data for the artificial benchmark.

## Benchmark Suite

The `bench/` directory contains comprehensive benchmarking tools.
After successful compilation, you find these executables in `cmake-build-release`:

### `./benchAll`

```bash
benchCE

Usage:  [options] [ce|ssb_sf1|ssb_sf10]

Options:
-b         Enable bottom inserts
-c         Cache the count aggregates
-f <value> Filter the query name (regex).
-help      Print this help message.
-i         Inline first element
-n         Enable naive merging strategy.
-p         Parallel Execution
-v         Validate the query result with the values given in the query files.
-m <value> Execution Mode (see bench/bench.cpp for all modes)
-r <value> Number of measured runs
-w <value> Number of warmup runs
```

Used to run the CE and the SSB benchmark.
The executable can be run with the following commands:

You can execute, e.g., the SSB benchmark factorized with scale factor 10, 5 warmup runs and 10 repetitions with caching, bottom-inserts and inlining enabled in parallel as follows:
```shell
./benchAll ssb_sf10 -bcip -w0 -r1 -mCodegenFactorized
make benchAll
./benchAll ssb_sf10 -bcip -w 5 -r 10 -mCodegenFactorizedPregen
```
In the first run, the code is generated.
Then it is compiled and, finally, the pregenerated code is executed in the second run.
Note that the second command executes all queries so far generated, i.e., maybe also queries from previous runs.

<div style="background:#fff3cd; border-left:4px solid #ffc107; padding:10px">
    Note that this script expects, that the project was properly set up.
    You may want to do this with the corresponding script as explained in the section Setup.
</div>


### `./benchArtificial`
The artificial benchmark is used for the microbenchmark evaluation in the paper.
We have different benchmark modes, for the paper we used `scaleUniform` and `alphaZipf`.

The script has similar options as `benchAll`:

```bash
benchCE

Usage:  [options] [args]

Options:
-b         Enable bottom inserts
-a         Runs all micro-benchmarks (including non-paper ones)
-g         Generates the input tables.
--help     Print this help message.
-k         Generate Bash Skript for queries
-m <value> Execution Mode (see bench/artificial/bench.cpp for all modes)
-p         Parallel Execution
-t <value> Benchmark Type (scaleUniform, ...)
-r <value> Number of measured runs
-w <value> Number of warmup runs
```

You can run the uniform queries of the paper flat and parallel as follows:
```bash
./benchArtificial -tscaleUniform -mCodegenFlat -w0 -r1 -p -g -kg
make benchArtificial
./benchArtificial -tscaleUniform -mCodegenFactorizedPregen -w5 -r10 -p -b 
```

### Reproduce experiments:
The experiments used in the paper can be executed using the commands above.
To run all targets for the experiments, we provide the script `bench/scripts/fact_paper_all.sh`.
After successfully running all experiments, you can generate the plots using `make`.

If the project was successfully set up and built, you can run the experiments as follows.
The scripts expect a running python environment with the requirements installed.


```bash
./bench/scripts/fact_paper_all.sh
make
```

Note that the exection of these experiments take a while.
Especially the single threaded CE experiments for the ablation study **can take several days**.

You can find results generated by the experiments in the folder `bench/results/raw`.
We provide the raw results used for the paper under `bench/results/paper`.
You can evaluate these by moving them to the folder `bench/results/raw`.

## Testing

The project includes unit tests built with Google Test:

```shell
./tester  # Run all tests
```
In addition, we provide for many benchmarks the option `-v`, which reads the result size from the query.
Then, it compares at the end of each query this value with the computed output.

## Architecture

FactDB uses a modular architecture:

1. **Parser Layer**: SQL string → AST (using Bison/Flex)
2. **Optimization Layer**: AST → Optimized Query Plan (with cost-based decisions)
3. **Code Generation**: Query Plan → Compiled C++ code (JIT compilation)
4. **Execution Layer**: Direct in-memory execution with specialized operators
5. **Result Handling**: Streaming results with performance tracking

## Dependencies

- **Google Test**: Unit testing framework
- **fmt**: String formatting library
- **Intel TBB**: Multi-threaded execution
- **PostgreSQL libpq**: Database client library
- **Bison/Flex**: SQL parser generation
- **SQLite3**: Embedded database for plan generation