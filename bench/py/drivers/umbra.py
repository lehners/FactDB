"""
Driver for Umbra (via Docker).

Umbra is special in two ways:
  1. It manages its own warmup/repeat cycle internally via \\set warmup / \\set repeat,
     so we override should_use_runner_loop() to return False and handle everything
     inside run_query() in a single Docker call.
  2. Setup requires two separate Docker runs: one to create the DB, one to load data.

Requires Docker to be available and the image to be pulled:
  docker pull umbradb/umbra:25.07.1

"""

import os
import re
import shutil
import subprocess
from pathlib import Path

from bench.py.driver import BenchmarkDriver


class UmbraDriver(BenchmarkDriver):
    image = "umbradb/umbra:25.07.1"
    db_path = "/var/db/umbra.db"
    umbra_sql = "umbra-sql"
    db_local_path = Path("db/umbra")

    # ------------------------------------------------------------------
    # BenchmarkDriver interface
    # ------------------------------------------------------------------
    def _docker_run(self, *args: str, strin: str|None = None, filestream: str | None = None, check: bool = True, do_log: bool = False) -> subprocess.CompletedProcess:
        if strin is not None:
            with self._log() as log:
                return subprocess.run(
                    [*args],
                    input=strin,
                    stdout=log if do_log else None,
                    stderr=subprocess.STDOUT,
                    text=True,
                    check=check,
                )
        else:
            assert filestream is not None, "Either string or filestream must be provided"
            with open(filestream, "r") as f, self._log() as log:
                return subprocess.run(
                    [*args],
                    stdin=f,
                    stdout=log if do_log else None,
                    stderr=subprocess.STDOUT,
                    text=True,
                    check=check,
                )

    def _volume(self):
        cwd = Path.cwd()
        return f"{cwd}/{self.db_local_path}"

    def setup(self) -> None:
        db_dir = Path(self._volume())
        if db_dir.exists():
            shutil.rmtree(db_dir)
        db_dir.mkdir(parents=True)

        print(self.image)
        cwd = Path.cwd()
        print(self._volume())

        # Create a fresh DB
        result = self._docker_run(
            "docker", "run", "--init", "-i", "-v", f"{self._volume()}:/var/db", self.image, self.umbra_sql, "-createdb", self.db_path,
            filestream=self._schema()
        )
        print(result.stdout)
        print("---")
        # Load data
        print(self._data_path)
        print(self._load())
        data_mount = f"{cwd}/{self._data_path}:/var/data/{self._data_path}"
        print(data_mount)
        result = self._docker_run(
            "docker", "run", "--init", "-i", "-v", f"{self._volume()}:/var/db", "-v", data_mount, "-w", "/var/data", self.image, self.umbra_sql, self.db_path,
            filestream=self._load()
        )
        print(result)
        print("setup done")

    def run_query(self) -> str:
        """
        Umbra handles its own warmup + repeat loop, so this single call
        returns the output for ALL runs. extract_time() must then aggregate
        the individual timings reported in that output.
        """
        cwd = Path.cwd()
        query_mount = f"{cwd}/{self._query_path}:/var/queries/{self._query_path}"
        print(query_mount)


        # Build the heredoc payload — Umbra's meta-commands set repeat/warmup
        sql_input = (
            f"\\set repeat {self._runs}\n"
            f"\\set warmup {self._warmups}\n"
            f"\\i /var/queries/{self._query()}\n"
        )

        # docker run --init -i -v umbra-db-art:/var/db -v $(pwd)/microbenchmarks:/var/microbenchmarks umbradb/umbra:25.07.1 umbra-sql /var/db/umbra.db <<EOF
        result = self._docker_run(
            "docker", "run", "--init", "-i", "-v", f"{self._volume()}:/var/db", "-v", query_mount, "-w", "/var/queries/", self.image, self.umbra_sql, self.db_path,
            strin=sql_input, do_log=True
        )
        print(result.stdout)
        print(result.stderr)
        return result.stderr

    def extract_time(self, output: str) -> float:
        """
        Because Umbra runs all repetitions internally, its output contains
        multiple timing lines. We return the mean of all of them.

        Adjust the regex below to match the actual timing format Umbra prints.
        Common formats:
          Time: 123.45 ms
          Execution time: 123.45 ms
        """
        pass

    # ------------------------------------------------------------------
    # Override: skip the outer warmup/run loop — Umbra does it internally
    # ------------------------------------------------------------------

    def should_use_runner_loop(self) -> bool:
        return False


DRIVER = UmbraDriver()
