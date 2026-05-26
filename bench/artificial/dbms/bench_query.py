import sys
import kuzu
from bench.performanceRecord import PerformanceRecord

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
    db = kuzu.Database(db_path)
    conn = kuzu.Connection(db)

    query = read_query(query_path)

    run_times, compile_times = [], []
    for i in range(warmups):
        result: kuzu.QueryResult = conn.execute(query)
        while result.has_next():
            res = result.get_next()
            if len(res) == 1:
               print(res[0])
            else:
               print(res)
        print("------------------")
    for i in range(runs):
        result: kuzu.QueryResult = conn.execute(query)
        while result.has_next():
            res = result.get_next()
            if len(res) == 1:
               print(res[0])
            else:
               print(res)
            #print(result.get_next())
        print("------------------")

        run_times.append(result.get_execution_time())
        compile_times.append(result.get_compiling_time())

    pr = PerformanceRecord()
    pr.compilation.from_times(compile_times, warmups)
    pr.execution.from_times(run_times, warmups)
    pr.execution.unit = 'ms'
    pr.compilation.unit = 'ms'

    pr.print_record()


if __name__ == '__main__':
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <db_path> <query_path>")
        sys.exit(1)  # Exit with an error code

    benchmark_query(sys.argv[1], sys.argv[2])

