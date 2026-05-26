#!/usr/bin/env bash
set -euo pipefail

# ---------------------------------------------------------------------------
# Usage:
#   ./run_ce.sh <mode> [warmups] [runs] [-- <extra args>]
#   ./run_ce.sh <mode> [warmups] [runs] --parts [-- <extra args>]
#   ./run_ce.sh <mode> [warmups] [runs] [--parts] [--notify] [-- <extra args>]
#
# Examples:
#   ./run_ce.sh CodegenFactorized 5 10 -- -pvbic
#   ./run_ce.sh CodegenFactorized 5 10 --parts -- -pvbic
#   ./run_ce.sh CodegenFactorized 5 10 --parts --notify -- -pvbic
# ---------------------------------------------------------------------------

BUILD_DIR="build"
GEN_DIR="factDB/gen"

usage() {
    echo "Usage: $0 <mode> [warmups] [runs] [--parts] [--notify] [-- <extra args>]"
    exit 1
}

# --- parse args -------------------------------------------------------------
[[ $# -lt 1 ]] && usage

MODE="$1"; shift
WARMUPS="${1:-5}";  [[ $# -gt 0 ]] && shift
RUNS="${1:-10}";    [[ $# -gt 0 ]] && shift

PARTS=false
if [[ "${1:-}" == "--parts" ]]; then
    PARTS=true
    shift
fi

notify=false
if [[ "${1:-}" == "--notify" ]]; then
    notify=true
    shift
fi

EXTRA=()
if [[ "${1:-}" == "--" ]]; then
    shift
    EXTRA=("$@")
fi

# --- notify setup -----------------------------------------------------------
dt=$(date '+%d/%m/%Y %H:%M:%S')

if $notify; then
    ~/mattermost_notifier/run.sh "CE benchmark ($MODE) started ($dt)."
fi

on_error() {
    local exit_code=$?
    local line_no=$1
    if $notify; then
        ~/mattermost_notifier/run.sh "❌ CE benchmark ($MODE) FAILED (exit $exit_code) at line $line_no"
    fi
}

trap 'on_error $LINENO' ERR

# --- output files -----------------------------------------------------------
OUT_DIR="bench/results/raw/ce"
mkdir -p "$OUT_DIR"

if [[ ${#EXTRA[@]} -gt 0 ]]; then
    EXTRA_SUFFIX="_$(echo "${EXTRA[*]}" | tr -s ' ' '_' | tr -d '-')"
else
    EXTRA_SUFFIX=""
fi

BASE_NAME="${OUT_DIR}/${MODE}${EXTRA_SUFFIX}"

OUT_FILE="${BASE_NAME}.txt"
if [[ -f "$OUT_FILE" ]]; then
    echo "Error: output file '$OUT_FILE' already exists. Remove it to re-run."
    exit 1
fi

# --- helpers ----------------------------------------------------------------
restore_gen() {
    echo "==> Restoring $GEN_DIR"
    git restore "$GEN_DIR"
}

build_bench() {
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make -j"$(nproc)" benchAll
}

# --- initial build ----------------------------------------------------------
restore_gen
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

echo "==> Initial build"
build_bench

# --- codegen + compile ------------------------------------------------------
EXE="${BUILD_DIR}/benchAll_${MODE}"

echo "==> Generating query files: -m${MODE}"
./benchAll ce -m"${MODE}" -r1 -w0 "${EXTRA[@]}"
echo "==> Rebuilding"
build_bench
cd ..
mv "${BUILD_DIR}/benchAll" "$EXE"
echo "==> Saved executable: $EXE"
restore_gen
cd "$BUILD_DIR"

# --- run --------------------------------------------------------------------
echo "==> Running: -m${MODE}Pregen -> $OUT_FILE"
cd ..

if [[ "$PARTS" == true ]]; then
  filter="^(?!(watdiv_acyclic_211_18|dblp_acyclic_218_11|dblp_acyclic_211_00|watdiv_acyclic_216_10|watdiv_acyclic_215_12|dblp_acyclic_205_14|dblp_acyclic_218_07|dblp_acyclic_218_15|watdiv_acyclic_217_01|watdiv_acyclic_217_11|watdiv_acyclic_217_13|watdiv_acyclic_217_17|watdiv_acyclic_218_04|watdiv_acyclic_218_11|watdiv_acyclic_218_12|dblp_acyclic_216_00).*)"

  echo "Run dblp part 1"
  "$EXE" ce -m"${MODE}Pregen" -w"${WARMUPS}" "-f${filter}dblp_acyclic_2(?:0[1-9]|1[0-6])" -r"${RUNS}" "${EXTRA[@]}" &> "$BASE_NAME.dblp1.txt"
  echo "Run dblp part 2"
  "$EXE" ce -m"${MODE}Pregen" -w"${WARMUPS}" "-f${filter}dblp(?!_acyclic_2(?:0[1-9]|1[0-6])).*" -r"${RUNS}" "${EXTRA[@]}" &> "$BASE_NAME.dblp2.txt"
  echo "Run epinions"
  "$EXE" ce -m"${MODE}Pregen" -w"${WARMUPS}" -f${filter}epinions.* -r"${RUNS}" "${EXTRA[@]}" &> "$BASE_NAME.epinions.txt"
  echo "Run hetio"
  "$EXE" ce -m"${MODE}Pregen" -w"${WARMUPS}" -f${filter}hetio.* -r"${RUNS}" "${EXTRA[@]}" &> "$BASE_NAME.hetio.txt"
  echo "Run job"
  "$EXE" ce -m"${MODE}Pregen" -w"${WARMUPS}" -f${filter}job.* -r"${RUNS}" "${EXTRA[@]}" &> "$BASE_NAME.job.txt"
  echo "Run watdiv"
  "$EXE" ce -m"${MODE}Pregen" -w"${WARMUPS}" -f${filter}watdiv.* -r"${RUNS}" "${EXTRA[@]}" &> "$BASE_NAME.watdiv.txt"
  echo "Run yago"
  "$EXE" ce -m"${MODE}Pregen" -w"${WARMUPS}" -f${filter}yago.* -r"${RUNS}" "${EXTRA[@]}" &> "$BASE_NAME.yago.txt"

  echo "unify parts into $OUT_FILE"
  cat "$BASE_NAME".{dblp1,dblp2,epinions,hetio,job,watdiv,yago}.txt > "$OUT_FILE"
else
  "$EXE" ce -m"${MODE}Pregen" -w"${WARMUPS}" -r"${RUNS}" "${EXTRA[@]}" &> "$OUT_FILE"
fi

echo "==> Done. Results written to $OUT_FILE"

if $notify; then
    ~/mattermost_notifier/run.sh "CE benchmark ($MODE) started at $dt is now complete. Results: $OUT_FILE"
fi