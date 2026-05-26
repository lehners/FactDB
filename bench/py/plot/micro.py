import os
import re
import pandas as pd
from bench.py.settings import *

QUERY_RE = re.compile(r"(.+)_(\d+)x(\d+)x(\d+)(?:_(\d+(?:[o.]\d+)?))?$")


def extract_x(queryname: str, subtype: str) -> float | None:
    m = QUERY_RE.match(queryname)
    if not m:
        return None
    v1, v2, v3 = int(m.group(2)), int(m.group(3)), int(m.group(4))
    scale = m.group(5)
    if subtype in ['diagonalZipf', 'diagonalUniform']:
        return v1
    elif subtype == 'scaleUniform':
        return float(scale.replace("o", ".")) if scale else None
    elif subtype == "alphaZipf":
        return float(scale.replace("o", ".")) if scale else None
    return None


def plot_subtype(input_dir: str, subtype: str, csv_outpath: str):
    os.makedirs(csv_outpath, exist_ok=True)
    for fname in sorted(os.listdir(input_dir)):
        if not fname.endswith(".csv"):
            continue

        df = pd.read_csv(os.path.join(input_dir, fname))
        system_name = fname.removesuffix(".csv")

        rows = []
        for _, row in df.iterrows():
            x = extract_x(row["queryname"], subtype)
            if x is not None:
                rows.append((x, row[REPORTED_VALUE]))

        if not rows:
            continue

        out_df = pd.DataFrame(rows, columns=["x", REPORTED_VALUE]).sort_values("x")
        out_path = os.path.join(csv_outpath, f"{system_name}.csv")
        out_df.to_csv(out_path, index=False, header=False)
        print(f"Wrote {out_path}")


def generate_csvs():
    in_dir = os.path.join(PARSED_DIR, "micro")
    out_dir = os.path.join(CSV_DIR, "micro")

    os.makedirs(out_dir, exist_ok=True)

    subtypes = [
        "diagonalUniform",
        "scaleUniform",
        "diagonalZipf",
        "alphaZipf",
    ]

    for subtype in subtypes:
        full_path = os.path.join(in_dir, subtype)
        if not os.path.exists(full_path):
            print(f"Skipping {subtype} — path not found.")
            continue
        plot_subtype(full_path, subtype, os.path.join(out_dir, subtype))


if __name__ == '__main__':
    generate_csvs()
