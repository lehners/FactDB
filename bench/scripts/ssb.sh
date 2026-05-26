#!/usr/bin/env bash
set -euo pipefail

# ---------------------------------------------------------------------------
# Usage:
#   ./run_ssb.sh <scale_factor> [warmups] [runs] [--only factorized|flat]
#
# scale_factor (required): e.g. 1, 10, 100
# warmups  (default: 5)
# runs     (default: 10)
# ---------------------------------------------------------------------------

usage() {
    echo "Usage: $0 <scale_factor> [warmups] [runs] [--only factorized|flat]"
    exit 1
}

# --- parse args -------------------------------------------------------------
[[ $# -lt 1 ]] && usage

SF="$1";        shift
WARMUPS="${1:-5}";  [[ $# -gt 0 ]] && shift
RUNS="${1:-10}";    [[ $# -gt 0 ]] && shift

ONLY=""
if [[ "${1:-}" == "--only" ]]; then
    shift
    ONLY="${1:?'--only requires an argument: fact|naive|flat'}"
    shift
fi

if [[ -n "$ONLY" ]] && ! echo "fact flat naive" | grep -qw "$ONLY"; then
    echo "Error: --only must be one of: factorized, flat, naive"
    usage
fi

# --- output files -----------------------------------------------------------
OUT_DIR="bench/results/raw/ssb/sf${SF}"
mkdir -p "$OUT_DIR"

OUT_FACT="${OUT_DIR}/fact.txt"
OUT_FLAT="${OUT_DIR}/flat.txt"
OUT_NAIVE="${OUT_DIR}/naive.txt"

check_exists() {
    local file="$1" step="$2"
    if [[ -z "$ONLY" || "$ONLY" == "$step" ]]; then
        if [[ -f "$file" ]]; then
            echo "Error: output file '$file' already exists. Remove it to re-run."
            exit 1
        fi
    fi
}
check_exists "$OUT_FACT"       "fact"
check_exists "$OUT_FLAT"       "flat"
check_exists "$OUT_NAIVE"      "naive"

# --- generate query files ---------------------------------------------------
echo "==> Generating query files"
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc) benchAll

./benchAll "ssb_sf${SF}" -mCodegenFactorized -r1 -w0 -pvbic
./benchAll "ssb_sf${SF}" -mCodegenFlat -r1 -w0 -pvbic
./benchAll "ssb_sf${SF}" -mCodegenFactorized -r1 -w0 -pvbicn

echo "==> Rebuild exe"
make -j$(nproc) benchAll
# --- run benchmarks ---------------------------------------------------------
run_step() {
    local step="$1" outfile="$2"; shift 2
    [[ -n "$ONLY" && "$ONLY" != "$step" ]] && return
    echo "==> Running: $step"
    ./benchAll "ssb_sf${SF}" "$@" &> ../"$outfile"
}

run_step fact  "$OUT_FACT"   -mCodegenFactorizedPregen -w"${WARMUPS}" -r"${RUNS}" -pvcib
run_step flat  "$OUT_FLAT"   -mCodegenFlatPregen       -w"${WARMUPS}" -r"${RUNS}" -pvbic
run_step naive "$OUT_NAIVE"  -mCodegenFactorizedPregen -w"${WARMUPS}" -r"${RUNS}" -pvcibn

echo "==> Done. Results written to $OUT_DIR"
