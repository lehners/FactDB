import os
import math
import pandas as pd
import numpy as np
from scipy.stats import gmean
from bench.py.plot import util
from bench.py.settings import *


BASELINE = "CodegenFactorized_pvbic"
FLAT = "CodegenFlat_pvbic"
FACT_NAIVE = "CodegenFactorized_pvbicn"
SPECIAL_SYSTEMS = [FLAT, FACT_NAIVE, "umbra_le", "umbra_result", "duckdb_result"]

def derive_stats(systems: dict[str, pd.Series], filter_keys: set[str] | None = None):
    if BASELINE not in systems:
        print(f"Missing baseline '{BASELINE}', skipping stats.")
        return

    # Align all systems on common queries
    df = pd.DataFrame(systems)
    if filter_keys:
        df = df[df.index.isin(filter_keys)]
    df_all = df.dropna()  # only queries run by all systems
    # df = df[df[FLAT] > 10] # filter queries which are running at least 10ms

    print(f"Found {len(df)} queries executed by at least one system.")
    print(f"Found {len(df_all)} queries executed by all system.")
    if df.empty:
        return

    baseline_times = df[BASELINE]

    def print_stats(system: str):
        speedups = df[system] / baseline_times # other / fact = speedup of fact over other
        arr = speedups.dropna().values
        print(f"----")
        print(f"  {system}: {len(arr)} queries")
        print(f"  {gmean(arr):.3f}x geomean speedup of fact vs. {system}")
        print(f"  {arr.max():.3f}x max,  {arr.min():.3f}x min")
        print(f"  {(arr >= 1).sum()} queries where fact is faster (>=1x)")
        print(f"  {(arr >= 10).sum()} queries where fact is >=10x faster")

    for s in df.columns:
        #if s != BASELINE:
        print_stats(s)


def main():
    input_dir = os.path.join(PARSED_DIR, "ce")
    systems = util.load_systems(input_dir)

    filters = {
        "acyclic": lambda q: "_acyclic" in q,
        "cyclic":  lambda q: "_cyclic" in q,
        "all":     lambda q: True,
    }
    for filter_name, pred in filters.items():
        keys = set()
        for df in systems.values():
            keys.update(q for q in df.index if pred(q))
        keys = {q for q in next(iter(systems.values())).index if pred(q)}

        print("---------------------------------------------------")
        print(f"                    {filter_name}")
        print("---------------------------------------------------")
        derive_stats(systems, keys)


if __name__ == "__main__":
    main()