import statistics
from enum import Enum
import re
import sys


class PerformanceValueEnum(Enum):
    Min = 4
    Max = 5
    Median = 6
    RelMAD = 7
    Avg = 8
    Sdev = 9


class PerformanceRecordType(Enum):
    Compilation = 1
    Execution = 2
    TotalTime = 3


class PerformanceValues:
    unit: str = "s"
    warmups: int = 0
    runs: int = 0
    min: float
    max: float
    median: float
    relMAD: float
    avg: float
    sdev: float

    def __init__(self, match: re.Match[str]=None, offset: int=None):
        if match is not None:
            assert(offset is not None)
            self.min = float(match.group(offset + 0))
            self.max = float(match.group(offset + 1))
            self.median = float(match.group(offset + 2))
            self.relMAD = float(match.group(offset + 3))
            self.avg = float(match.group(offset + 4))
            self.sdev = float(match.group(offset + 5))

    def read_runs_and_unit(self, match: re.Match[str], offset: int):
        self.unit = match.group(offset + 0)
        self.warmups = int(match.group(offset + 1))
        self.runs = int(match.group(offset + 2))

    def get(self, val: PerformanceValueEnum) -> float:
        if val == PerformanceValueEnum.Min:
            return self.min
        elif val == PerformanceValueEnum.Max:
            return self.max
        elif val == PerformanceValueEnum.Median:
            return self.median
        elif val == PerformanceValueEnum.RelMAD:
            return self.relMAD
        elif val == PerformanceValueEnum.Avg:
            return self.avg
        elif val == PerformanceValueEnum.Sdev:
            return self.sdev

    def normalize(self):
        scaler = 1
        if self.unit == "s":
            scaler = 1e3
        elif self.unit == "ms":
            scaler = 1e0
        elif self.unit == "µs":
            scaler = 1e-3
        elif self.unit == "ns":
            scaler = 1e-6
        else:
            raise "unknown unit detected"

        self.min *= scaler
        self.max *= scaler
        self.median *= scaler
        self.avg *= scaler
        self.unit = 'ms'

    def from_times(self, times_list: [float], warmups):
        self.warmups = warmups
        self.runs = len(times_list)
        self.min = min(times_list)
        self.max = max(times_list)
        self.median = statistics.median(times_list)
        self.relMAD = statistics.mean([abs(i - self.median) for i in times_list])
        self.avg = statistics.mean(times_list)
        self.sdev = 0 if len(times_list) <= 1 else statistics.stdev(times_list)

    def initEmpty(self):
        self.warmups = 0
        self.runs = 0
        self.min = 0
        self.max = 0
        self.median = 0
        self.relMAD = 0
        self.avg = 0
        self.sdev = 0
        self.unit = 's'

    def get_info_str(self, count_infos: bool):
        formatted_string = "{:.7f} min, {:7f} max, {:7f} median, {:2f}% relMAD, {:7f} avg, {:7f} sdev".format(
            self.min, self.max, self.median, self.relMAD, self.avg, self.sdev
        )
        if count_infos:
            return f'{self.warmups} warmups, {self.runs} runs, {formatted_string}'
        return formatted_string

    def to_dict(self):
        return {
            "unit": self.unit,
            "warmups": self.warmups,
            "runs": self.runs,
            "min": self.min,
            "max": self.max,
            "median": self.median,
            "relMAD": self.relMAD,
            "avg": self.avg,
            "sdev": self.sdev,
        }


class PerformanceRecord:
    execution: PerformanceValues
    compilation: PerformanceValues

    sub_records: [PerformanceValues]

    def __init__(self, match: re.Match[str]=None, mode='factDB'):
        self.sub_records = []
        if match is None:
            self.execution = PerformanceValues()
            self.compilation = PerformanceValues()
        elif mode == 'factDB':
            self.execution = PerformanceValues(match, 1+3)
            self.compilation = PerformanceValues(match, 10+3)
            self.execution.read_runs_and_unit(match, 1)
            self.compilation.read_runs_and_unit(match, 10)
        elif mode == 'Umbra':
            self.execution = PerformanceValues(match, 1)
            self.compilation = PerformanceValues(match, 18)
        elif mode == 'Umbra2':
            self.execution = PerformanceValues(match, 1)
            self.compilation = PerformanceValues(match, 8)

    @staticmethod
    def from_runtimes(runtimes: list[float], warmups: int = 0):
        """Create a record from runtime samples.

        The provided runtimes populate the execution statistics. Compilation
        is initialized as empty so callers can fill it later if needed.
        """
        record = PerformanceRecord()
        record.execution.from_times(runtimes, warmups)
        record.compilation.initEmpty()
        record.execution.unit = "ms"
        return record

    def get(self, record_type: PerformanceRecordType, record_value: PerformanceValueEnum) -> float:
        if record_type == PerformanceRecordType.Compilation:
            return self.compilation.get(record_value)
        elif record_type == PerformanceRecordType.Execution:
            return self.execution.get(record_value)
        elif record_type == PerformanceRecordType.TotalTime:
            return self.execution.get(record_value) + self.compilation.get(record_value)

    def normalize(self):
        self.execution.normalize()
        self.compilation.normalize()
        return self

    def add_sub_record(self, matches: re.Match[str]):
        self.sub_records.append(PerformanceValues(matches, 1))

    def to_dict(self):
        self.normalize()
        dict = {}
        for k, v in self.execution.to_dict().items():
            dict[f'execution_{k}'] = v
        for k, v in self.compilation.to_dict().items():
            dict[f'compilation_{k}'] = v
        return dict

    def print_record(self, count_infos=True, file=sys.stderr):
        print(f' [{self.execution.unit}] execution: ({self.execution.get_info_str(count_infos)}) '
              f'[{self.compilation.unit}] compilation: ({self.compilation.get_info_str(count_infos)})', file=file)

    def _format_record(self, count_infos: bool = True) -> str:
        return (
            f" [{self.execution.unit}] execution: ({self.execution.get_info_str(count_infos)}) "
            f"[{self.compilation.unit}] compilation: ({self.compilation.get_info_str(count_infos)})"
        )

    def __str__(self) -> str:
        return self._format_record(True)

    def print_record(self, count_infos=True, file=sys.stderr):
        print(self._format_record(count_infos), file=file)