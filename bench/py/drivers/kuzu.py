"""
Driver for KùzuDB (via kuzu_con.py).

Setup:
  - Drops and recreates the database at db/kuzu/artificial.db
  - Loads schema and data from <query_path>/load.cypher

Run:
  - Executes microbenchmarks/queries/queryLeftDeep.cypher with --repeat
"""

import os
import re
import shutil
import subprocess
import kuzu
from pathlib import Path

from fontTools.t1Lib import write

from bench.py.driver import BenchmarkDriver

DB_PATH    = Path("db/kuzu")


class KuzuDriver(BenchmarkDriver):
    _kuzu_db = None
    _kuzu_conn = None
    _exec_time = None
    _comp_time = None

    def print_result(self, result):
        with self._log() as log:
            while result.has_next():
                res = result.get_next()
                if len(res) == 1:
                    log.write(f'{res[0]}\n')
                else:
                    log.write(f'{res}\n')
        print("------------------")

    def setup(self) -> None:
        db = DB_PATH / self._db_name
        if db.exists():
            shutil.rmtree(DB_PATH)
        os.makedirs(DB_PATH, exist_ok=True)

        nthreads = os.cpu_count() or 1
        self._kuzu_db = kuzu.Database(db)
        self._kuzu_conn = kuzu.Connection(self._kuzu_db, num_threads=nthreads)

        with open(self._schema()) as query_file:
            for line in query_file:
                print(line)
                if not line.strip():
                    continue
                if line.startswith("--"):
                    continue
                else:
                    self._kuzu_conn.execute(line.strip())

        with open(self._load()) as query_file:
            for line in query_file:
                print(line)
                if not line.strip():
                    continue
                if line.startswith("--"):
                    continue
                else:
                    self._kuzu_conn.execute(line.strip())

    def run_query(self) -> str:
        query = ""
        with open(self._query()) as query_file:
            for line in query_file:
                if not line.strip():
                    continue
                if line.startswith("--"):
                    continue
                else:
                    query += " " + line.strip()

        results = self._kuzu_conn.execute(query)
        self.print_result(results)
        self._comp_time = results.get_compiling_time()
        self._exec_time = results.get_execution_time()
        with self._log() as log:
            log.write(f'TIME - comp time: {self._comp_time}ms.\n')
            log.write(f'TIME - exec time: {self._exec_time}ms.\n')
        return "Nothing to return for Kuzu"

    def extract_time(self, output: str) -> float:
        return self._exec_time

    def _format(self) -> str:
        return "cypher"


DRIVER = KuzuDriver()