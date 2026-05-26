import os
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from bench.py.plot import util
from bench.py import settings


ABLATION_BASELINE = "CodegenFlatLeftDeep_v"

def plot_ablation(save_plot: bool = True):
    input_dir = os.path.join(settings.PARSED_DIR, "ce")
    if not os.path.exists(input_dir):
        return
    csv_outpath = os.path.join(settings.CSV_DIR, "ce/ablation")
    os.makedirs(csv_outpath, exist_ok=True)

    systems = util.load_systems(input_dir)
    print(systems)
    if ABLATION_BASELINE not in systems:
        print(f"Missing ablation baseline '{ABLATION_BASELINE}', skipping.")
        return

    df = pd.DataFrame(systems)#.dropna()
    baseline = df[ABLATION_BASELINE]

    for system_name, times in df.items():
        speedups = np.sort((baseline / times).values)
        fractions = np.arange(len(speedups))

        out_df = pd.DataFrame({"fraction": fractions, "speedup": speedups})
        out_path = os.path.join(csv_outpath, f"cdf_{system_name}.csv")
        out_df.to_csv(out_path, index=False)
        print(f"Wrote {out_path}")

        if save_plot:
            plt.plot(fractions, speedups, linewidth=2, label=system_name)

    if save_plot:
        plt.yscale("log")
        plt.axhline(y=1, linestyle="--", linewidth=1)
        plt.xlabel("Fraction of queries", fontsize=12)
        plt.ylabel("Speedup over flat execution", fontsize=12)
        plt.grid(True, which="both", linestyle="--", alpha=0.5)
        plt.legend(fontsize=12, title="Optimization Step", title_fontsize=13)
        plt.tight_layout()
        plt.show()
        #plt.savefig(os.path.join(csv_outpath, "ablation.pdf"), bbox_inches="tight")
        print("Saved ablation.pdf")


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("--no-plot", action="store_true")
    args = parser.parse_args()
    plot_ablation(save_plot=not args.no_plot)