#!/bin/bash

inPath="generator_configs"
outPath="generated_tests"

traffic_sim_variant=$1 # description of the currently compiled traffic sim (i.e. SIMD, parallel,...)
output_file="bench-all-results.csv"

function generateTests {
	inDirectory="${inPath}/${1}"
	outDirectory="${outPath}/${1}"

	eval "mkdir -p ${outDirectory}" # create outDirectory if it does not exist

	for filename in "${inDirectory}/*.json"; do
		eval "python3 gen.py -n 10 --output-path '${outDirectory}/${filename}.{seed}.json' ${inDirectory}/{$filename}.json"
	done
}

# hardcoded for all input paths
# generateTests <subdirectory in tools/generator_configs/>