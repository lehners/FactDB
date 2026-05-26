import csv
import os
import re
from bench.py.settings import *
from bench.py.performance_record import PerformanceRecord


def read_result_file(filename: str) -> dict[str, PerformanceRecord]:
    # returns the performance record with unit ms
    results_dict: dict[str, PerformanceRecord] = {}
    queryname = ""
    prevQueryname = None

    regex_queryname = re.compile(r"queryname: ([a-zA-Z_0-9.]*)")
    regex_execution = re.compile(
        r" \[(.+)\] execution: \((\d+) warmups, (\d+) runs, (\d*\.\d+) min, (\d*\.\d+) max, (\d*\.\d+) median, (\d*\.\d+)% relMAD, (\d*\.\d+) avg, (\d*\.\d+) sdev\) \[(.+)\] compilation: \((\d+) warmups, (\d+) runs, (\d*\.\d+) min, (\d*\.\d+) max, (\d*\.\d+) median, (\d*\.\d+)% relMAD, (\d*\.\d+) avg, (\d*\.\d+) sdev\)")
    regex_sub_record = re.compile(
        r"--\d+\((\d*\.\d+) min, (\d*\.\d+) max, (\d*\.\d+) median, (\d*\.\d+)% relMAD, (\d*\.\d+) avg, (\d*\.\d+) sdev\)")
    regex_execution_umbra = re.compile(
        r"INFO:\s+\[s\] execution: \((\d*\.\d+) min, (\d*\.\d+) max, (\d*\.\d+) median, (\d*\.\d+)% relMAD, (\d*\.\d+) avg, (\d*\.\d+) sdev, (\d*\.\d+) cycles, (\d*\.\d+) instr, (\d*\.\d+) L1D-misses(?:, (\d*\.\d+) LLC-misses)?, (\d*\.\d+) DTLB-misses, (\d*\.\d+) branch-misses, (\d*\.\d+) task, (\d+) scale, (\d*\.\d+) IPC, (\d*\.\d+) CPUs, (\d*\.\d+) GHz\) compilation: \((\d*\.\d+) min, (\d*\.\d+) max, (\d*\.\d+) median, (\d*\.\d+)% relMAD, (\d*\.\d+) avg, (\d*\.\d+) sdev\)"
    )
    regex_execution_umbra2 = re.compile(
        r"INFO:\s+\[s\] execution: \((\d*\.\d+) min, (\d*\.\d+) max, (\d*\.\d+) median, (\d*\.\d+)% relMAD, (\d*\.\d+) avg, (\d*\.\d+) sdev, (\d+) scale, nan IPC, nan CPUs, nan GHz\) compilation: \((\d*\.\d+) min, (\d*\.\d+) max, (\d*\.\d+) median, (\d*\.\d+)% relMAD, (\d*\.\d+) avg, (\d*\.\d+) sdev\)"
    )

    def processTime(regex, mode):
        match = regex.match(curLine)
        if match:
            if queryname in results_dict and prevQueryname not in results_dict:  # try to patch wrong umbra output
                results_dict[prevQueryname] = results_dict[queryname]
                results_dict.pop(queryname)
            elif queryname in results_dict:
                print(f"Duplicate {queryname} in {filename}")
            assert (queryname not in results_dict)
            results_dict[queryname] = PerformanceRecord(match, mode).normalize()
            return True
        return False

    with open(filename) as file:
        for curLine in file:
            match1 = regex_queryname.match(curLine)
            if match1:
                prevQueryname = queryname
                queryname = match1.group(1)
                continue

            if processTime(regex_execution, 'factDB'):
                continue
            elif processTime(regex_execution_umbra, 'Umbra'):
                continue
            elif processTime(regex_execution_umbra2, 'Umbra2'):
                continue

            curLine = curLine.strip()
            match3 = regex_sub_record.match(curLine)
            if match3:
                assert (queryname in results_dict)
                results_dict[queryname].add_sub_record(match3)
                continue

    return results_dict


def write_parsed_csv(filename: str, results_dict: dict[str, PerformanceRecord]) -> None:
    rows: list[dict[str, object]] = []
    fieldnames = ["source", "queryname"]

    os.makedirs(os.path.dirname(filename), exist_ok=True)

    for queryname, record in results_dict.items():
        row = {"source": os.path.basename(filename), "queryname": queryname}
        row.update(record.to_dict())
        rows.append(row)

        for key in row:
            if key not in fieldnames:
                fieldnames.append(key)

    with open(filename, "w", newline="") as csv_file:
        writer = csv.DictWriter(csv_file, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(rows)


def parse_all():
    for dirpath, _, files in os.walk(RAW_DIR):
        if dirpath.startswith("bench/results/parsed"):
            continue
        print(dirpath)
        for filename in files:
            file_path = os.path.join(dirpath, filename)
            if not os.path.isfile(file_path):
                continue
            illegal_suffixes = [".dblp1.txt", ".dblp2.txt", ".epinions.txt", ".hetio.txt", ".job.txt", ".watdiv.txt",
                                ".yago.txt", ".DS_Store"]
            if any(str(filename).endswith(suffix) for suffix in illegal_suffixes):
                continue

            res = read_result_file(file_path)

            outfile = os.path.join(dirpath.replace(RAW_DIR, PARSED_DIR, 1), filename)
            outfile = os.path.splitext(outfile)[0] + ".csv"
            write_parsed_csv(outfile, res)


if __name__ == '__main__':
    parse_all()
    print("done")
