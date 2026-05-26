#!/usr/bin/env python3
"""
Example experiment script.

Shows how thin per-benchmark code becomes once the runner handles
all the boilerplate.  Run from the repo root:

    python3 microbenchmarks/run_experiment.py
"""

import subprocess
import sys

DRIVERS = ["duckdb", "fbench", "kuzu", "lmfao", "umbra"]

def run_query(driver: str, data_path: str, query_path: str, out_dir: str, query_name: str, first: bool) -> None:
    cmd = [
        sys.executable, "-m", "bench.py.run",
        driver,
        data_path,
        query_path,
        out_dir,
        "--queryname", f'{query_name}',
        "--warmups", "5",
        "--runs", "10",
    ]
    if not first:
        cmd.append("--may_exist")
    subprocess.run(
        cmd,
        check=True,
    )


def run_helper(queries, out_dir):
    for driver in DRIVERS:
        try:
            first = True
            for (query_name, data_path) in queries:
                run_query(driver, data_path, "bench/artificial/queries", out_dir, query_name, first)
                first = False
        except subprocess.CalledProcessError as e:
            if e.returncode == 2:
                print(f"driver {driver} timed out, do not execute it for subsequent queries")
                continue
            else:
                raise e
