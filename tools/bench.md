# Benchmarker

The benchmarker is a utility for measuring and comparing the run time of
several executions with a number input files.

The benchmarker is loosely integrated with the generator. When using the
recommended output path format, the benchmarker extracts information about the
scenario and the seed. The format is `scenario_name.{seed}.json`. Outputs in
this format can be generated using:
```bash
python3 gen.py -n 100 --output-path 'generated_tests/bench/mini_bench_v1.{seed}.json' mini_bench_v1.json
```

## Using the Benchmarker

The benchmarker requires Python >= 3.6 to run.

The `-h` flag displays detailed help texts:
```bash
python3 bench.py -h
```

Run the executable `../traffic_sim` once for every file in the
`generated_tests/bench/` directory and write the results to `results.csv`:
```bash
python3 bench.py ../traffic_sim generated_tests/bench/ >results.csv
```

The benchmarker supports features for merging the output CSV files from
different executables or executable variations. The arguments
`--header-prefix` and `--row-prefix` prepend an arbitrary number of columns to
the header or content rows. The values passed to the arguments are interpreted
as CSV.

```bash
# Initialise headers
python3 bench.py --header-only --header-prefix "kind" ir relevant >results.csv
# Append results for kind = sequential
python3 bench.py --no-header --row-prefix "sequential" ../traffic_sim_seq generated_tests/bench/ >>results.csv
# Append results for kind = parallel
python3 bench.py --no-header --row-prefix "parallel" ../traffic_sim_par generated_tests/bench/ >>results.csv
```

