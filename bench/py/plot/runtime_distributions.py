import os
import pandas as pd
from bench.py.settings import *
from bench.py.plot import util
import argparse

BASELINE = "CodegenFactorized_pvbic"


def write_runtime_distribution(systems: dict[str, pd.Series], query_filter: str, csv_outpath: str,
                               filter_keys: set[str] | None = None):
    if BASELINE not in systems:
        print(f"Missing baseline system '{BASELINE}', skipping runtime distribution.")
        print(f"The available systems are {systems.keys()}")

        return

    baseline = systems[BASELINE]
    if filter_keys:
        baseline = baseline[baseline.index.isin(filter_keys)]

    for system_name, times in systems.items():
        if system_name == BASELINE:
            continue
        if filter_keys:
            times = times[times.index.isin(filter_keys)]

        combined = pd.DataFrame({"other": times, "fact": baseline}).dropna()
        out_path = os.path.join(csv_outpath, f"runtime_distribution{query_filter}_{system_name}.csv")
        combined.to_csv(out_path, index=False, header=False)
        print(f"Wrote {out_path}")


def mainCE():
    input_dir = os.path.join(PARSED_DIR, "ce")
    csv_outpath = os.path.join(CSV_DIR, "ce/runtimes")
    os.makedirs(csv_outpath, exist_ok=True)
    systems = util.load_systems(input_dir)

    filters = {
        "_acyclic": lambda q: "_acyclic" in q,
        "_cyclic":  lambda q: "_cyclic" in q,
        "_all":     lambda q: True,
    }
    for filter_name, pred in filters.items():
        keys = set()
        for df in systems.values():
            keys.update(q for q in df.index if pred(q))

        write_runtime_distribution(systems, filter_name, csv_outpath, keys or None)

def mainSSB(scale_factor):
    input_dir = os.path.join(PARSED_DIR, f"ssb/sf{scale_factor}")
    csv_outpath = os.path.join(CSV_DIR, f"ssb/sf{scale_factor}")
    os.makedirs(csv_outpath, exist_ok=True)
    if not os.path.exists(input_dir):
        return
    systems = util.load_systems(input_dir)

    keys = set()
    for df in systems.values():
        keys.update(q for q in df.index)

    write_runtime_distribution(systems, "", csv_outpath, keys or None)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Run benchmark pipelines")

    parser.add_argument(
        "mode",
        choices=["ce", "ssb_sf1", "ssb_sf10"],
        help="Which benchmark to run"
    )

    args = parser.parse_args()

    if args.mode == "ce":
        mainCE()
    elif args.mode == "ssb_sf1":
        BASELINE = "fact"
        mainSSB(1)
    elif args.mode == "ssb_sf10":
        BASELINE = "fact"
        mainSSB(10)