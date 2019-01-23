# Generator

The generator is capable of generating input files for the simulator and
optimiser. It uses a configuration file containing the methods and
distributions used for generating and populating the grid. The generator uses
a _seed_ to initialise the pseudo random number generator. This allows
replicating the simulator input files by specifying the same configuration
file and seed.

## Using the Generator

The generator requires Python >= 3.5 to run.

The `-h` flag displays detailed help texts:
```bash
python3 gen.py -h
```

The simplest way to call the generator is for producing just one output file
(usable as an input file for the simulator). The seed is printed to stderr:
```bash
python3 gen.py myconfig_v1.json > myconfig_v1.test.json
```

The generator can also generate many output files at once from the same
configuration file, each output file with a different seed. In this mode, an
output path containing a variable must be specified. Special care must be
taken with shell escaping, single quotes (`'`) are recommended for the output
path:
```bash
python3 gen.py -n 1000 --output-path 'generated_tests/mini_bench_v1.{seed}.json' mini_bench_v1.json
```

## Writing Configuration Files

The generator takes a configuration file as its inputs.

Some considerations when writing these files:

 * For a `m x n` grid:
   * `no of roads = 2 (m * n - 2)`
   * `no of rfbs = 2 * no of roads`
   * `no of lanes = avg no of lanes per road * no of rfbs`
