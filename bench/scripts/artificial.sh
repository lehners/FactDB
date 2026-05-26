#!/usr/bin/env bash
set -euo pipefail

# ---------------------------------------------------------------------------
# Usage:
#   ./run_bench.sh <benchmark_type> [warmups] [runs] [--only <step>]
#
# benchmark_type (required):
#   growing1NoDups | diagonalUniform | diagonalZipf |
#   scaleUniform   | scaleZipf       | alphaZipf
#
# warmups  (default: 5)
# runs     (default: 10)
#
# --only <step>  run only one step instead of all three:
#   factorized | flat | flat_pregen
# ---------------------------------------------------------------------------

VALID_TYPES="growing1NoDups diagonalUniform diagonalZipf scaleUniform scaleZipf alphaZipf"

usage() {
    echo "Usage: $0 <benchmark_type> [warmups] [runs] [--only flat|ti|bi]"
    echo "Valid benchmark types: $VALID_TYPES"
    exit 1
}

# --- parse args -------------------------------------------------------------
[[ $# -lt 1 ]] && usage

BENCH_TYPE="$1"; shift
WARMUPS="${1:-5}";  [[ $# -gt 0 ]] && shift
RUNS="${1:-10}";    [[ $# -gt 0 ]] && shift

ONLY=""
if [[ "${1:-}" == "--only" ]]; then
    shift
    ONLY="${1:?'--only requires an argument: flat|ti|bi'}"
    shift
fi

# validate benchmark type
if ! echo "$VALID_TYPES" | grep -qw "$BENCH_TYPE"; then
    echo "Error: unknown benchmark type '$BENCH_TYPE'"
    usage
fi

# validate --only value
if [[ -n "$ONLY" ]] && ! echo "flat ti bi" | grep -qw "$ONLY"; then
    echo "Error: --only must be one of: flat, ti, bi"
    usage
fi

# --- output files -----------------------------------------------------------
OUT_DIR="bench/results/raw/micro/${BENCH_TYPE}"
mkdir -p "$OUT_DIR"

OUT_FLAT="${OUT_DIR}/flat.txt"
OUT_TI="${OUT_DIR}/top_insert.txt"
OUT_BI="${OUT_DIR}/bottom_insert.txt"

# check existing outputs for selected steps
check_exists() {
    local file="$1" step="$2"
    if [[ -z "$ONLY" || "$ONLY" == "$step" ]]; then
        if [[ -f "$file" ]]; then
            echo "Error: output file '$file' already exists. Remove it to re-run."
            exit 1
        fi
    fi
}
check_exists "$OUT_FLAT" "flat"
check_exists "$OUT_TI"   "ti"
check_exists "$OUT_BI"   "bi"

# --- build ------------------------------------------------------------------
echo "==> Building benchmark"
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release .. 
make -j"$(nproc)" benchArtificial

# --- check if data already exists -------------------------------------------
if [ ! -d "bench/data/micro" ]; then
  echo "Query files do not exist in bench/data/micro. Please generate first with `./scripts/bench/setup.sh`"
fi

# --- generate query files ---------------------------------------------------
echo "==> Generating query files"
./benchArtificial -tdiagonalUniform -mCodegenFlat          -r1 -w0 -p -a
./benchArtificial -tdiagonalUniform -mCodegenFactorized    -r1 -w0 -p -a -b
./benchArtificial -tdiagonalUniform -mCodegenFactorized    -r1 -w0 -p -a

echo "==> Rebuilding after codegen"
make -j"$(nproc)" benchArtificial

# --- run benchmarks ---------------------------------------------------------
run_step() {
    local step="$1" cmd=("${@:2}")
    [[ -n "$ONLY" && "$ONLY" != "$step" ]] && return
    echo "==> Running: $step"
    "${cmd[@]}"
}

run_step ti ./benchArtificial -mCodegenFactorizedPregen -w"${WARMUPS}" -r"${RUNS}" -p    -t"${BENCH_TYPE}" &> "../$OUT_TI"

run_step bi ./benchArtificial -mCodegenFactorizedPregen -w"${WARMUPS}" -r"${RUNS}" -p -b -t"${BENCH_TYPE}" &> "../$OUT_BI"

run_step flat ./benchArtificial -mCodegenFlatPregen     -w"${WARMUPS}" -r"${RUNS}" -p  -t"${BENCH_TYPE}" &> "../$OUT_FLAT"

echo "==> Done. Results written to $OUT_DIR"