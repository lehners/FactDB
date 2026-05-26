#!/usr/bin/env python3

from bench.py.experiment.util.microbenchmark_driver import *

QUERIES = [
    ("join123_100x100x100", "./bench/data/micro/zipf_100x100x100_100_1"),
    ("join123_1000x1000x1000", "./bench/data/micro/zipf_1000x1000x1000_100_1"),
    ("join123_2000x2000x2000", "./bench/data/micro/zipf_2000x2000x2000_100_1"),
    ("join123_3000x3000x3000", "./bench/data/micro/zipf_3000x3000x3000_100_1"),
    ("join123_5000x5000x5000", "./bench/data/micro/zipf_5000x5000x5000_100_1"),
    ("join123_10000x10000x10000", "./bench/data/micro/zipf_10000x10000x10000_100_1"),
    ("join123_15000x15000x15000", "./bench/data/micro/zipf_15000x15000x15000_100_1"),
    ("join123_20000x20000x20000", "./bench/data/micro/zipf_20000x20000x20000_100_1"),
]

OUTDIR="bench/results/raw/micro/diagonalZipf"


def run():
    run_helper(QUERIES, OUTDIR)


if __name__ == "__main__":
    run()

