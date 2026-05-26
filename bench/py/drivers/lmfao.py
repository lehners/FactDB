"""
Driver for LMFAO.

Setup:
  - Runs multifaq to generate code from the query path
  - Compiles the generated C++ runtime with make -j

Run:
  - Executes the compiled ./lmfao binary from LMFAO/runtime/cpp

Time extraction:
  - Delegates to microbenchmarks/dbms/lmfao_extract_times.py
    (same logic as before, but called as a library function rather than
     a subprocess — see extract_time() below)
"""

import os
import re
import sys
import subprocess
from pathlib import Path

from bench.py.driver import BenchmarkDriver

LMFAO_DIR   = Path("LMFAO")
RUNTIME_DIR = LMFAO_DIR / "runtime" / "cpp"
BINARY      = Path(".") / RUNTIME_DIR / "lmfao"

class LmfaoDriver(BenchmarkDriver):
    def setup(self) -> None:
        if not LMFAO_DIR.exists():
            print("LMFAO not found, make sure to run ./scripts/bench/load_fdb.sh first.", file=sys.stderr)
            exit(1)

        # Generate code
        subprocess.run(
            ["./multifaq", "--path", f"{os.path.join(os.getcwd(), self._data_path)}", "--model", "count"],
            cwd=LMFAO_DIR,
            check=True,
        )
        # Compile
        jobs = str(os.cpu_count() or 8)
        subprocess.run(
            ["make", f"-j{jobs}"],
            cwd=RUNTIME_DIR,
            check=True,
        )

    def run_query(self) -> str:
        with self._log() as log:
            result = subprocess.run(
                [str(BINARY)],
                stdout=subprocess.PIPE,   # capture for return value...
                stderr=log,               # ...but log stderr directly
                text=True,
                check=True,
            )
        # Also append stdout to the log after capturing it
        with self._log() as log:
            log.write(result.stdout)

        return result.stdout

    def extract_time(self, output: str) -> float:
        # Extract times
        times = re.findall(r'Data \w+: (\d+)ms', output)
        times_sum = sum(int(t) for t in times)

        return times_sum


DRIVER = LmfaoDriver()