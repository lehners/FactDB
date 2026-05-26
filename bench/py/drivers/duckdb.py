"""
Driver for DuckDB (via kuzu_con.py).

Setup:
  - Drops and recreates the database at db/duckdb/artificial.db
  - Loads schema and data from <query_path>/load.sql

"""

import os
import sys
import shutil
import threading
import time
from bench.py.performance_record import PerformanceRecord

import duckdb
from pathlib import Path

from bench.py.driver import BenchmarkDriver

DB_PATH = Path("db/duckdb")


class DuckDBDriver(BenchmarkDriver):
    _duckdb_db = None
    _duckdb_conn = None
    _exec_time = {}

    def execute(self, query: str, repeat):
        if not repeat:
            print(query)
        if self._queryname is not None and repeat:
            self._exec_time[self._queryname] = []
            self.log(f'queryname: {self._queryname}')

        timer = None
        if self._timeout > 0:
            def interrupt():
                self._duckdb_conn.interrupt()

            timer = threading.Timer((self._warmups + self._runs) * self._timeout, interrupt)
            timer.start()
        try:
            # Warm-up runs
            for i in range(self._warmups if repeat else 0):
                result = self._duckdb_conn.execute(query).fetchall()
                for row in result:
                    self.log(f'{row[0] if len(row) == 1 else row}')
                self.log("------------------")

            # Timed runs
            for i in range(self._runs if repeat else 1):
                start = time.time()
                result = self._duckdb_conn.execute(query).fetchall()
                end = time.time()

                for row in result:
                    self.log(f'{row[0] if len(row) == 1 else row}')
                self.log("------------------")

                if repeat:
                    self._exec_time[self._queryname].append((end - start) * 1000)
                    self.log(f'DuckDB Runtime: {(end - start) * 1000}')

        except duckdb.InterruptException as err:
            print(f"Query was interrupted: {err}")
            if timer is not None:
                timer.cancel()
                timer.join()
            sys.exit(2)  # Exit without error

        if repeat:
            pr = PerformanceRecord.from_runtimes(self._exec_time[self._queryname], self._warmups)
            with self._log() as log:
                pr.print_record(file=log)

        if timer is not None:
            timer.cancel()
            timer.join()


    def bench_queries(self, query_filepath, repeat):
        print(f"process file: {query_filepath}")
        if not os.path.exists(query_filepath):
            print("query path not found!")
            exit(1)
        with open(query_filepath) as query_file:
            seen_query = ""
            for line in query_file:
                if not line.strip():
                    continue
                if line.startswith("--"):
                    continue
                elif line.startswith("\\q"):
                    return
                elif line.startswith("\\set queryname "):
                    assert (not seen_query)
                    self._queryname = line[15:-1].strip()
                    self.log(f"\\queryname {self._queryname}")
                    print(f'queryname: {self._queryname}')
                elif line.startswith("\\i "):
                    assert (not seen_query)
                    self.bench_queries(line[2:-1].strip(), repeat)
                elif line.startswith("\\show queryname"):
                    print(f'queryname: {self._queryname}')
                elif line.startswith("\\"):
                    assert (not seen_query)
                    print(f"unknown backslash command: {line.strip()}")
                elif line.strip().endswith(";"):
                    seen_query += line
                    try:
                        self.execute(seen_query.strip(), repeat)
                    except duckdb.IOException as err:
                        print(err)
                    seen_query = ""
                else:
                    seen_query += " " + line

    def setup(self) -> None:
        if not DB_PATH.exists():
            os.makedirs(DB_PATH)
        db = DB_PATH / self._db_name
        if db.exists():
            if db.is_file():
                db.unlink()
            elif db.is_dir():
                shutil.rmtree(db)

        self._duckdb_conn = duckdb.connect(db)
        self.bench_queries(self._schema(), False)
        self.bench_queries(self._load(), False)

    def run_query(self) -> str:
        self.bench_queries(self._query(), True)

    def extract_time(self, output: str):
        return self._exec_time

    def should_use_runner_loop(self) -> bool:
        return False


DRIVER = DuckDBDriver()
