import os
import matplotlib.pyplot as plt
from bench.performanceRecord import *
import matplot2tikz
import csv

csv_outpath = "result/csv/"

def savefig(path):
    # plt.tight_layout(pad=1.02)
    dir = os.path.dirname(path)
    if not os.path.exists(dir):
        os.makedirs(dir)

    tikzPath = path.rsplit( ".", 1 )[0] + ".pgf"
    matplot2tikz.save(tikzPath)
    print(tikzPath)
    plt.savefig(path, bbox_inches='tight', pad_inches=0.1, dpi=300)


def plot_data(ax, x_values, y_values, lbls, x_lbl, path):
    # plt.figure(figsize=(8, 6))  # Optional: Set the figure size
    store_result = False
    if not ax:
        store_result = True
        fig, ax = plt.subplots()
        fig.set_size_inches(6.0, 3.0)  # width x height in inches

    for idx, y_vals in enumerate(y_values):
        lbls[idx] = lbls[idx].replace("_", "")
        ax.plot(x_values, y_vals, marker='o', markersize=1, linestyle='-', color=colors[idx % len(colors)], label=lbls[idx])

    # Add labels and title
    ax.set_xlabel(x_lbl)
    # ax.set_ylabel('Time [ms]')
    ax.set_yscale('log')
    ax.legend(loc='upper center', ncol=4)

    # Show grid and the plot
    ax.grid(True)
    if store_result:
        savefig(path)

def write_csv(lbls, x_vals, curves, subtype):
    if not csv_outpath:
        return
    subtype_prefix = re.search("^([A-Za-z]*)", subtype).group()
    for lbl, curve in zip(lbls, curves):
        outpath = os.path.join(csv_outpath, f"{subtype_prefix}_{lbl}.csv")
        print(f'write csv to {outpath}')
        with open(outpath, 'w') as csvfile:
            value_writer = csv.writer(csvfile)
            for i in zip(x_vals, curve):
                value_writer.writerow(i)

def plot_diagonal(ax, query_infos, results, lbls, output_path, subtype):
    assert(len(lbls) == len(results))
    curves = [[] for i in range(len(results))]
    x_vals = []
    for i in query_infos.keys():
        cur_info = query_infos[i]
        for v1, v2, v3, scale in cur_info:
            for inner_idx in range(len(results)):
                benchmark_name = f'{i}_{v1}x{v2}x{v3}'
                if benchmark_name not in results[inner_idx]:
                    curves[inner_idx].append(float('nan'))
                else:
                    curves[inner_idx].append(results[inner_idx][benchmark_name].execution.avg)
            x_vals.append(v1)
    if ax:
        ax.set_xlim([0, max(x_vals)])
    write_csv(lbls, x_vals, curves, subtype)

    # Create the plot
    plot_data(ax, x_vals, curves, lbls, 'number of tuples in R_1, R_2, and R_3', output_path)

def plot_growing_relation(ax, grow_id, query_infos, results, lbls, output_path, subtype):
    assert(len(lbls) == len(results))
    curves = [[] for i in range(len(results))]
    x_vals = []
    fixed_vals = None
    for i in query_infos.keys():
        cur_info = query_infos[i]
        for v1, v2, v3, scale in cur_info:
            for inner_idx in range(len(results)):
                curves[inner_idx].append((results[inner_idx][f'{i}_{v1}x{v2}x{v3}'].execution.avg))
            if grow_id == 1:
                x_vals.append(v1)
                newFixed = v2, v3
            elif grow_id == 2:
                x_vals.append(v2)
                newFixed = v1, v3
            elif grow_id == 3:
                x_vals.append(v3)
                newFixed = v1, v2
            else:
                raise RuntimeError("Expected either 1, 2, or 3 for table 1,2,3...")

            assert(fixed_vals is None or newFixed == fixed_vals)
            fixed_vals = newFixed
    if ax:
        ax.set_xlim([0, max(x_vals)])
    write_csv(lbls, x_vals, curves, subtype)
    # Create the plot
    plot_data(ax, x_vals, curves, lbls, f'number of tuples in R_{grow_id}, other relations const', output_path.format(grow_id))

def plot_growing_scale(ax, query_infos, results, lbls, output_path, subtype):
    assert(len(lbls) == len(results))
    curves = [[] for i in range(len(results))]
    x_vals = []
    fixed_vals = None
    for i in query_infos.keys():
        cur_info = query_infos[i]
        for v1, v2, v3, scale in cur_info:
            for inner_idx in range(len(results)):
                queryname = f'{i}_{v1}x{v2}x{v3}_{scale}'
                if queryname not in results[inner_idx]:
                    queryname = queryname[:-2]
                if queryname not in results[inner_idx]:
                    curves[inner_idx].append((0))
                else:
                    curves[inner_idx].append((results[inner_idx][queryname].execution.avg))
            x_vals.append(scale)
            new_fixed = v1, v2, v3

            assert(fixed_vals is None or new_fixed == fixed_vals)
            fixed_vals = new_fixed
    if ax:
        ax.set_xlim([0, max(x_vals)])
    write_csv(lbls, x_vals, curves, subtype)
    # Create the plot
    plot_data(ax, x_vals, curves, lbls, f'number of duplicates per relation, relation size const', output_path)

def plot_alpha(ax, query_infos, results, lbls, output_path):
    assert(len(lbls) == len(results))
    curves = [[] for i in range(len(results))]
    x_vals = []
    fixed_vals = None
    for i in query_infos.keys():
        cur_info = query_infos[i]
        for v1, v2, v3, alpha in cur_info:
            for inner_idx in range(len(results)):
                queryname = f'{i}_{v1}x{v2}x{v3}_{alpha}'
                if queryname not in results[inner_idx]:
                    queryname = queryname[:-2]
                if queryname not in results[inner_idx]:
                    curves[inner_idx].append((0))
                else:
                    curves[inner_idx].append((results[inner_idx][queryname].execution.avg))
            x_vals.append(alpha)
            new_fixed = v1, v2, v3

            assert(fixed_vals is None or new_fixed == fixed_vals)
            fixed_vals = new_fixed
    if ax:
        ax.set_xlim([0, max(x_vals)])
    write_csv(lbls, x_vals, curves, 'alpha')
    plot_data(ax, x_vals, curves, lbls, f'alpha of zipf', output_path)


def plot(input_path, subtype, ax=None):
    print(f"Try to plot {input_path}")
    if not subtype:
        filename = os.path.basename(input_path)
        if filename.startswith("art_"):
            subtype = filename[4:]

    files = prefer_fact_and_flat(input_path)
    
    results = []
    lbls = []
    for file in files:
        filepath = os.path.join(input_path, file)
        if not os.path.isfile(filepath):
            continue
        if not os.path.exists(filepath):
            continue
        if file.endswith(".pdf") or file.endswith(".cout.txt") or file.endswith(".svg") or file.endswith(".pgf"):
            continue
        results.append(read_result_file(filepath))
        lbls.append(file)

    query_infos = {}
    keyRegex = re.compile(r"(.+)_(\d+)x(\d+)x(\d+)(?:_(\d+(?:\.\d+)?))?")
    for key in results[0].keys():
        match = keyRegex.match(key)
        if match:
            queryname = match.group(1)
            if queryname not in query_infos:
                query_infos[queryname] = []
            scale = match.group(5)
            scale = float(scale) if scale else None
            tuple_to_insert = (int(match.group(2)), int(match.group(3)), int(match.group(4)), scale)
            query_infos[queryname].append(tuple_to_insert)
        else:
            print(f"unkown key: {key}")

    print(query_infos)

    if subtype.startswith("diagonal"):
        plot_diagonal(ax, query_infos, results, lbls, os.path.join(input_path, "diag.svg"), subtype)
    elif subtype.startswith("growing"):
        idx = int(subtype[7])
        plot_growing_relation(ax, idx, query_infos, results, lbls, os.path.join(input_path, "growing_relation{}.svg"), subtype)
    elif subtype.startswith("scale"):
        plot_growing_scale(ax, query_infos, results, lbls, os.path.join(input_path, "scale.svg"), subtype)
    elif subtype.startswith("alpha"):
        plot_alpha(ax, query_infos, results, lbls, os.path.join(input_path, "alpha.svg"))
    else:
        print(f"unkown subtype: {subtype}")

def do_plots(input_path, subtype):
    plot(input_path, subtype)

def do_paper_plots():
    paths = [
        "result/xeon1/paperready/art_diagonalUniform_20250606_080708",
        "result/xeon1/paperready/art_scaleUniform_20250606_075027",
        "result/xeon1/paperready/art_diagonalZipf_20250606_082025",
        "result/xeon1/paperready/art_alphaZipf_20250606_082629",
    ]
    fig, axs = plt.subplots(2, 2, figsize=(8, 7), gridspec_kw={'height_ratios': [1, 1]})
    # fig, axs = plt.subplots(2, len(paths)//2)

    plot_axes = axs[:2].flatten()
    for i, p in enumerate(paths):
        plot(p, "", plot_axes[i])
        plot_axes[i].legend().remove()  # Remove individual legends
        plot_axes[i].set_ylim([0, 10**4])

    plt.legend(loc='lower center', bbox_to_anchor=(0, -.75), ncol=4)

    fig.set_size_inches(12.0, 6.0)  # width x height in inches
    savefig("result/xeon1/paperready/fig.svg")
