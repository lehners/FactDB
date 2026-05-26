#!/usr/bin/env python3
"""
Example experiment script.

Shows how thin per-benchmark code becomes once the runner handles
all the boilerplate.  Run from the repo root:

    python3 microbenchmarks/run_experiment.py
"""
import os.path
import subprocess
import sys

DRIVERS = ["duckdb", "umbra", "umbra_le"]


def run(scale_factor:int):
    if scale_factor not in [1, 10]:
        print(f"SSB Benchmark not implemtented for SF {scale_factor}. Only implemented for 1 and 10 at the moment.")

    if not os.path.exists(f"bench/data/ssb/sf{scale_factor}"):
        print(f"Please make sure to generate the correct dataset for scale_factor {scale_factor} first!")
        exit(1)

    for driver in DRIVERS:
        subprocess.run(
            [
                sys.executable, "-m", "bench.py.run",
                driver,
                f"bench/data/ssb/sf{scale_factor}",
                f"bench/ssb/sf{scale_factor}",
                f"bench/results/raw/ssb/sf{scale_factor}",
                "--warmups", "5",
                "--runs", "10",
            ],
            check=True,
        )


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("--sf", type=int, default=10)
    args = parser.parse_args()
    run(args.sf)
