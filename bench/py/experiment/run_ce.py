#!/usr/bin/env python3
"""
Example experiment script.

Shows how thin per-benchmark code becomes once the runner handles
all the boilerplate.  Run from the repo root:

    python3 microbenchmarks/run_experiment.py
"""

import subprocess
import sys
import os

DRIVERS = ["duckdb", "umbra", "umbra_le"]


def run():
    if not os.path.exists("bench/data/ce"):
        print("Data was not provided. Pleas run ./bench/ce/setup.sh first")
        exit(1)
    for driver in DRIVERS:
        subprocess.run(
            [
                sys.executable, "-m", "bench.py.run",
                driver,
                "bench/data",
                "bench/ce",
                "bench/results/raw/ce",
                "--warmups", "5",
                "--runs", "10",
            ],
            check=True,
        )


if __name__ == "__main__":
    run()
