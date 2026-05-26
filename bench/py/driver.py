"""
BenchmarkDriver — the interface every DBMS driver must implement.

To add a new DBMS:
1. Create bench/drivers/<name>.py
2. Subclass BenchmarkDriver and implement setup(), run_query(), extract_time()
3. Expose a module-level DRIVER = MyDriver() instance
"""

from abc import ABC, abstractmethod
from dataclasses import dataclass, field
import os

@dataclass
class QueryResult:
    """Timing result for a single query."""
    query_name: str
    times_ms:   list[float]   # one entry per timed run
    warmups:    int

class BenchmarkDriver(ABC):

    def __init__(self) -> None:
        self._warmups:  int = 5
        self._runs:     int = 10
        self._timeout:  int = 0
        self._db_name:  str = "artificial.db"
        self._log_file: str = None
        self._queryname: str = None
        self._data_path: str  = None
        self._query_path: str = None
        self._driver: str = self.__class__.__module__.rsplit(".", 1)[-1]

    def _log(self):
        """Open log file in append mode (create if missing)."""
        return open(self._log_file, "a")

    def _format(self) -> str:
        return "sql"

    def _schema(self):
        return os.path.join(self._query_path, f"schema.{self._format()}")

    def _load(self):
        fmt = self._format()

        candidates = [
            os.path.join(self._query_path, f"load_{self._driver}.{fmt}"),
            os.path.join(self._query_path, f"load.{fmt}"),
            os.path.join(self._data_path, f"load_{self._driver}.{fmt}"),
            os.path.join(self._data_path, f"load.{fmt}"),
        ]

        for path in candidates:
            if os.path.isfile(path):
                return path

        return candidates[-1]

    def _query(self):
        fmt = self._format()

        candidates = [
            os.path.join(self._query_path, f"query_{self._driver}.{fmt}"),
            os.path.join(self._query_path, f"query.{fmt}"),
            os.path.join(self._query_path, f"all_queries_{self._driver}.{fmt}"),
            os.path.join(self._query_path, f"all_queries.{fmt}"),
        ]

        for path in candidates:
            if os.path.isfile(path):
                return path

        return candidates[-1]

    def configure(self, warmups: int, runs: int, queryname: str, log_file: str, data_path: str, query_path: str) -> None:
        """Called by the runner before setup() to pass the configured counts."""
        self._warmups = warmups
        self._runs    = runs
        self._log_file = log_file
        self._queryname = queryname
        self._data_path = data_path
        self._query_path = query_path

    @abstractmethod
    def setup(self) -> None:
        """
        Called once before any warmup or timed run.

        Use this for anything that should NOT be included in the timing:
        schema creation, data loading, binary compilation, etc.
        """

    @abstractmethod
    def run_query(self) -> str:
        """
        Execute the query once and return the raw stdout output as a string.

        This is called for both warmup and timed runs. Keep it side-effect-free
        across calls (i.e. don't mutate state that affects timing).
        """

    @abstractmethod
    def extract_time(self, output: str):
        """
        Parse the raw output from run_query() and return the elapsed time in ms.

        Raise ValueError with a descriptive message if the output cannot be parsed,
        rather than returning 0 or None — silent failures corrupt the results.

        For drivers where should_use_runner_loop() returns False, the output
        contains ALL repetitions. extract_time() should return a single
        representative value (e.g. mean) across all of them.
        """

    def should_use_runner_loop(self) -> bool:
        """
        Return True (default) if the shared runner should drive warmup and
        timed repetitions by calling run_query() in a loop.

        Return False for systems that manage their own repeat/warmup cycle
        internally (e.g. Umbra's \\set repeat / \\set warmup). In that case
        the runner calls setup() once, then run_query() once, then extract_time().
        The driver is responsible for honouring the warmup/runs counts passed
        to it at construction time.
        """
        return True

    def log(self, val: str):
        with self._log() as log:
            log.write(val)
            log.write("\n")
