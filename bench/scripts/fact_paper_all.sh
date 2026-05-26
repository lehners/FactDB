#!/usr/bin/env bash
set -euo pipefail

# ---------------------------------------------------------------------------
# Full benchmark pipeline:
#   1. Artificial benchmarks (Python runners + bash script)
#   2. CE benchmarks         (Python runner  + bash script)
#   3. SSB benchmark         (Python runner  + bash script)
# ---------------------------------------------------------------------------

dt=$(date '+%d/%m/%Y %H:%M:%S');

notify=false
for arg in "$@"; do
  if [[ "$arg" == "--notify" ]]; then
    notify=true
    ~/mattermost_notifier/run.sh "Benchmarking script for fact paper (world is not flat) started ($dt)."
    break
  fi
done

on_error() {
  local exit_code=$?
  local line_no=$1
  if $notify; then
    ~/mattermost_notifier/run.sh "❌ FAILED (exit $exit_code) at line $line_no"
  fi
}

trap 'on_error $LINENO' ERR

# --- pyenv ------------------------------------------------------------------
source ./venv/bin/activate

# --- artificial -------------------------------------------------------------
echo "==> [Artificial] Running Python experiments"
python3 -m bench.py.experiment.run_alphaZipf
python3 -m bench.py.experiment.run_scaleUniform

echo "==> [Artificial] Running bash benchmarks"
./bench/scripts/artificial.sh alphaZipf
./bench/scripts/artificial.sh scaleUniform

# --- CE ---------------------------------------------------------------------
echo "==> [CE] Running Python experiment"
python3 -m bench.py.experiment.run_ce

echo "==> [CE] Running bash benchmarks"
# add arguments below as needed:
./bench/scripts/ce.sh CodegenFlatLeftDeep 5 10 --parts -- -v
./bench/scripts/ce.sh CodegenFlat 5 10 --parts -- -pvbic
./bench/scripts/ce.sh CodegenFactorizedLeftDeep 5 10 --parts -- -v
./bench/scripts/ce.sh CodegenFactorizedLeftDeep 5 10 --parts -- -pv
./bench/scripts/ce.sh CodegenFactorizedLeftDeep 5 10 --parts -- -pvb
./bench/scripts/ce.sh CodegenFactorizedLeftDeep 5 10 --parts -- -pvbc
./bench/scripts/ce.sh CodegenFactorizedLeftDeep 5 10 --parts -- -pvbic
./bench/scripts/ce.sh CodegenFactorized 5 10 -- -pvbic
./bench/scripts/ce.sh CodegenFactorized 5 10 -- -pvbicn


# --- SSB ---------------------------------------------------------------------
echo "==> [SSB] Running Python experiment"
python3 -m bench.py.experiment.run_ssb --sf 10

echo "==> [SSB] Running bash benchmark"
./bench/scripts/ssb.sh 10

echo "==> All benchmarks done."

python3 -m bench.py.parse_raws

if $notify; then
  ~/mattermost_notifier/run.sh "Benchmarking script for fact paper (world is not flat) started at $dt is now complete."
fi
