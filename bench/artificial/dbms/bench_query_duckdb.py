import sys
import os
import duckdb
import threading
import time
from bench.performanceRecord import PerformanceRecord


timeout = 11.000 #

def read_query(query_path):
    res_str = ""
    with open(query_path, 'r') as file:
        for line in file:
            line = line.strip()
            if line.startswith("//") or not line:
                continue
            res_str += line + " "
    return res_str.strip()

def benchmark_query(db_path, query_path, warmups=5, runs=10):
    con = duckdb.connect(database=db_path)
    query = read_query(query_path)

    run_times = []
    compile_times = []

    timer = None
    if timeout > 0:
        def interrupt():
            con.interrupt()

        timer = threading.Timer((warmups + runs) * timeout, interrupt)
        timer.start()


    # Warm-up runs
    for i in range(warmups):
        result = con.execute(query).fetchall()
        for row in result:
            print(row[0] if len(row) == 1 else row)
        print("------------------")

    # Timed runs
    for i in range(runs):
        start = time.time()
        result = con.execute(query).fetchall()
        end = time.time()

        for row in result:
            print(row[0] if len(row) == 1 else row)
        print("------------------")

        run_times.append((end - start)*1000)
        compile_times.append(0)

    if timer is not None:
        timer.cancel()
        timer.join()

    # Results
    pr = PerformanceRecord()
    pr.compilation.from_times(compile_times, warmups)
    pr.execution.from_times(run_times, warmups)
    pr.execution.unit = 'ms'
    pr.compilation.unit = 'ms'

    pr.print_record()

