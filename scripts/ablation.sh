set -e

NOTIFY=false
for arg in "$@"; do
    if [ "$arg" = "--notify" ]; then
        NOTIFY=true
    fi
done

notify() {
    if [ "$NOTIFY" = true ]; then
        . ~/mattermost_notifier/venv/bin/activate
        ~/mattermost_notifier/run.sh
    fi
}

notify

cd cmake-build-release

make -j128 benchAll
mv benchAll benchAllBefore

mkdir -p ablation

# filter out queries which lead to problems (mainly OOM)
filter="^(?!(watdiv_acyclic_211_18|dblp_acyclic_218_11|dblp_acyclic_211_00|watdiv_acyclic_216_10|watdiv_acyclic_215_12|dblp_acyclic_205_14|dblp_acyclic_218_07|dblp_acyclic_218_15|watdiv_acyclic_217_01|watdiv_acyclic_217_11|watdiv_acyclic_217_13|watdiv_acyclic_217_17|watdiv_acyclic_218_04|watdiv_acyclic_218_11|watdiv_acyclic_218_12|dblp_acyclic_216_00).*)"

build_bench() {
    # Clear generated code and generate query code
    git restore ../factDB/gen/.
    ./benchAllBefore ce -r1 -w0 "-mCodegen$1" "-$2"

    # Build again
    make -j128 benchAll
    echo "Building done - start execution."

    mv benchAll "benchAll_$1_$2"
}

run_bench() {
    echo "Run bench_$1_$2"
    # Run the benchmarks
    echo "Run dblp part 1"
    "./benchAll_$1_$2" ce -r10 -w5 "-mCodegen$1Pregen" "-$2" "-f${filter}dblp_acyclic_2(?:0[1-9]|1[0-6])" &> "ablation/f_ablation$1_$2.dblp1.txt"
    echo "Run dblp part 2"
    "./benchAll_$1_$2" ce -r10 -w5 "-mCodegen$1Pregen" "-$2" "-f${filter}dblp(?!_acyclic_2(?:0[1-9]|1[0-6])).*" &> "ablation/f_ablation$1_$2.dblp2.txt"
    echo "Run epinions"
    "./benchAll_$1_$2" ce -r10 -w5 "-mCodegen$1Pregen" "-$2" -f${filter}epinions.* &> "ablation/f_ablation$1_$2.epinions.txt"
    echo "Run hetio"
    "./benchAll_$1_$2" ce -r10 -w5 "-mCodegen$1Pregen" "-$2" -f${filter}hetio.* &> "ablation/f_ablation$1_$2.hetio.txt"
    echo "Run job"
    "./benchAll_$1_$2" ce -r10 -w5 "-mCodegen$1Pregen" "-$2" -f${filter}job.* &> "ablation/f_ablation$1_$2.job.txt"
    echo "Run watdiv"
    "./benchAll_$1_$2" ce -r10 -w5 "-mCodegen$1Pregen" "-$2" -f${filter}watdiv.* &> "ablation/f_ablation$1_$2.watdiv.txt"
    echo "Run yago"
    "./benchAll_$1_$2" ce -r10 -w5 "-mCodegen$1Pregen" "-$2" -f${filter}yago.* &> "ablation/f_ablation$1_$2.yago.txt"
}


build_bench FlatLeftDeep v
build_bench FactorizedLeftDeep v
build_bench FactorizedLeftDeep vp
build_bench FactorizedLeftDeep vpb
build_bench FactorizedLeftDeep vpbc
build_bench FactorizedLeftDeep vpbci

echo "builds finished, let's run!"

run_bench FlatLeftDeep v
run_bench FactorizedLeftDeep v
run_bench FactorizedLeftDeep vp
run_bench FactorizedLeftDeep vpb
run_bench FactorizedLeftDeep vpbc
run_bench FactorizedLeftDeep vpbci

notify
