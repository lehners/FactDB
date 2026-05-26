import os
import pandas as pd
from bench.py.settings import *

def load_systems(input_dir: str) -> dict[str, pd.DataFrame]:
    """Returns {system_name: DataFrame indexed by queryname}."""
    systems = {}
    for fname in sorted(os.listdir(input_dir)):
        if not fname.endswith(".csv"):
            continue
        df = pd.read_csv(os.path.join(input_dir, fname))
        print(fname)
        if df.empty:
            continue
        systems[fname.removesuffix(".csv")] = df.set_index("queryname")[REPORTED_VALUE]
    return systems
