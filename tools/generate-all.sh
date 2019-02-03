#!/bin/bash
: "${GEN_N:=1}"

inPath="generator_configs"
outPath="generated_tests"

traffic_sim_variant=$1 # description of the currently compiled traffic sim (i.e. SIMD, parallel,...)
output_file="bench-all-results.csv"

function generateTests {
	inDirectory="${inPath}/${1}"
	outDirectory="${outPath}/${1}"

	eval "mkdir -p ${outDirectory}" # create outDirectory if it does not exist
	eval "rm -i ${outDirectory}/*"

	for filename in "${inDirectory}"/*.json; do
		name=$(basename "$filename" .json)
		eval "python3 gen.py -n ${GEN_N} --output-path '${outDirectory}/${name}.{seed}.json' ${inDirectory}/${name}.json"
	done
}

# hardcoded for all input paths
# generateTests <subdirectory in tools/generator_configs/>
generateTests "carAmount"
generateTests "gridSize"
generateTests "sparseScaling"
generateTests "streetLength"