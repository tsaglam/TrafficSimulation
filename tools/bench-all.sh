#!/bin/bash

traffic_sim_variant=$1 # description of the currently compiled traffic sim (i.e. SIMD, parallel,...)
output_file="bench-all-results.csv"

function runBenchmark {
	dimension=$1
	inputPath=$2
	prefix="\"${traffic_sim_variant}, ${dimension}\""
	eval "python3 bench.py --no-header --row-prefix ${prefix} ../traffic_sim ${inputPath} >> ${output_file}"
}

eval "python3 bench.py --header-only --header-prefix \"traffic_sim_variant, dimension\" ir relevant > ${output_file}"

# hardcoded for all input paths
# runBenchmark <dimension> <inputPath>
runBenchmark "carAmount" "generated_tests/carAmount"
runBenchmark "gridSize" "generated_tests/gridSize"
runBenchmark "sparseScaling" "generated_tests/sparseScaling"
runBenchmark "streetLength" "generated_tests/streetLength"