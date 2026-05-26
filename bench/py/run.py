#!/usr/bin/env python3
"""
Benchmark runner.

Usage:
    python3 -m bench.run <driver> <query_name> <query_path> [options]

Examples:
    python3 -m bench.run fbench q1 microbenchmarks/queries/q1
    python3 -m bench.run kuzu   q1 microbenchmarks/queries/q1 --warmups 3 --runs 5
    python3 -m bench.run lmfao  q1 microbenchmarks/queries/q1

Environment overrides:
    WARMUPS=3 RUNS=5 python3 -m bench.run fbench q1 path/to/q1
"""

import argparse
import importlib
import os
import sys
from bench.py.driver import BenchmarkDriver
from bench.py.performance_record import PerformanceRecord


def load_driver(name: str) -> BenchmarkDriver:
    try:
        module = importlib.import_module(f"bench.py.drivers.{name}")
    except ModuleNotFoundError:
        print(f"Error: no driver found for '{name}'.", file=sys.stderr)
        print(f"Expected a module at bench/py/drivers/{name}.py", file=sys.stderr)
        sys.exit(1)

    # Each driver module must expose a DRIVER instance
    if not hasattr(module, "DRIVER"):
        print(f"Error: bench/drivers/{name}.py must define a DRIVER instance.", file=sys.stderr)
        sys.exit(1)

    driver = module.DRIVER
    if not isinstance(driver, BenchmarkDriver):
        print(f"Error: DRIVER in bench/drivers/{name}.py must be a BenchmarkDriver subclass.", file=sys.stderr)
        sys.exit(1)

    return driver


def main() -> None:
    parser = argparse.ArgumentParser(description="Run a benchmark against a DBMS driver.")
    parser.add_argument("driver",     help="Driver name (e.g. fbench, kuzu, lmfao)")
    parser.add_argument("data_path",  help="Path to the data files")
    parser.add_argument("query_path", help="Path to the query directory or file")
    parser.add_argument("outdir",     type=str, help="Path to the folder with outputs.")
    parser.add_argument("--warmups",  type=int, default=int(os.environ.get("WARMUPS", 5)))
    parser.add_argument("--runs",     type=int, default=int(os.environ.get("RUNS",    10)))
    parser.add_argument("--queryname",type=str, default=None, help="the queryname if any")
    parser.add_argument("--may_exist",action="store_true", help="the output file may already exist")
    args = parser.parse_args()

    os.makedirs(args.outdir, exist_ok=True)
    out_file = os.path.join(args.outdir, f"{args.driver}.txt")
    if not args.may_exist and os.path.exists(out_file):
        print(f"Warning: output file {out_file} already exists.", file=sys.stderr)
        exit(1)

    driver = load_driver(args.driver)
    driver.configure(warmups=args.warmups, runs=args.runs, log_file=out_file, queryname=args.queryname, data_path=args.data_path, query_path=args.query_path)
    driver._timeout = 15

    driver.log("\n")
    driver.log(f"driver:    {args.driver}")
    driver.log(f"queryname: {args.queryname}")
    driver.log(f"querypath: {args.query_path}")
    driver.log(f"datapath:  {args.data_path}")
    driver.log(f"warmups:   {args.warmups}")
    driver.log(f"runs:      {args.runs}\n")

    # --- setup (schema creation, compilation, etc.) ---
    print("Setting up...", file=sys.stderr)
    # Let drivers that manage their own loop know the configured counts
    driver.setup()

    print("logFile", driver._log_file)

    if driver.should_use_runner_loop():
        # Normal path: runner drives warmup + timed repetitions
        driver.log(f"\\queryname {driver._queryname}")
        print(f"Warmup ({args.warmups} runs)...", file=sys.stderr)
        for i in range(args.warmups):
            print(f"  warmup #{i + 1}", file=sys.stderr)
            driver.run_query()
            driver.log("---\n")

        print(f"Measuring ({args.runs} runs)...", file=sys.stderr)
        times: list[float] = []
        for i in range(args.runs):
            print(f"  run #{i + 1}", file=sys.stderr)
            output = driver.run_query()
            t = driver.extract_time(output)
            times.append(t)
            print(f"  time: {t} ms", file=sys.stderr)
            driver.log("---")
        pr = PerformanceRecord.from_runtimes(times, args.warmups)
        driver.log(pr.__str__())
    else:
        # Driver-managed loop (e.g. Umbra): single call, extract aggregate
        print(f"Driver manages warmup/repeat internally — single invocation...", file=sys.stderr)
        output = driver.run_query()
        if output is not None:
            print(output)
        times = driver.extract_time(output)

    print(f"\nAll times (ms): {times}", file=sys.stderr)
    # PerformanceRecord.(args.warmups, times)


if __name__ == "__main__":
    main()