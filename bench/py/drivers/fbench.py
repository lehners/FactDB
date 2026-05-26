"""
Driver for fbench.

Expects:
  ./fbench binary in the working directory
  Output contains a line like: TIME - overall time: 123ms.
"""

import os
import re
import sys
import subprocess

from bench.py.driver import BenchmarkDriver


class FBenchDriver(BenchmarkDriver):

    def setup(self) -> None:
        if not os.path.exists("fbench"):
            print("FBench not found, make sure to run ./scripts/bench/load_fdb.sh first.", file=sys.stderr)
            exit(1)

        # check if correct files exist in path:
        assert(os.path.exists(os.path.join(self._data_path, "dtree.txt")))
        assert(os.path.exists(os.path.join(self._data_path, "schema.conf")))

    def run_query(self) -> str:
        nthreads = os.cpu_count() or 1
        with self._log() as log:
            result = subprocess.run(
                ["./fbench", "--path", self._data_path, "--threads", str(nthreads)],
                stdout=subprocess.PIPE,   # capture for return value...
                stderr=log,               # ...but log stderr directly
                text=True,
                check=True,
            )
            log.write(result.stdout)
        return result.stdout

    def extract_time(self, output: str) -> float:
        match = re.search(r"TIME - overall time:\s*([\d.]+)ms", output)
        if not match:
            raise ValueError(
                f"Could not find timing line in fbench output.\n"
                f"Expected pattern: 'TIME - overall time: <N>ms.'\n"
                f"Got:\n{output}"
            )
        return float(match.group(1))


DRIVER = FBenchDriver()