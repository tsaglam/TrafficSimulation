
import os
import csv
import io
import time
import itertools
import argparse
import subprocess
import sys


class Benchmarker(object):
    class Result(object):
        def __init__(self, input, run_time):
            super(Benchmarker.Result, self).__init__()
            self.input = input
            self.run_time = run_time

    def __init__(self, executable, inputs):
        super(Benchmarker, self).__init__()
        self._executable = executable
        self._inputs = inputs

    def _perform_run(self, input_path):
        with open(input_path) as f:
            start = time.monotonic()
            subprocess.run(
                [self._executable],
                stdin = f,
                stdout = subprocess.DEVNULL,
                stderr = subprocess.DEVNULL,
                check = True,
            )
            end = time.monotonic()
            return end - start

    def __iter__(self):
        for input in self._inputs:
            try:
                run_time = self._perform_run(input.path)

                yield Benchmarker.Result(input, run_time)
            except subprocess.CalledProcessError as e:
                print(
                    'Error occured during processing of {}, '
                    'returncode {}'.format(
                        input.input,
                        e.returncode,
                    ),
                    file=sys.stderr,
                )


class InputInfo(object):
    def __init__(self, path, input, scenario=None, seed=None):
        super(InputInfo, self).__init__()
        self.path = path
        self.input = input
        self.scenario = scenario
        self.seed = seed

    def csv_info(self):
        scenario_str = self.scenario if self.scenario is not None else ''
        seed_str = self.seed if self.seed is not None else ''

        return [self.input, scenario_str, seed_str]

    @staticmethod
    def csv_header():
        return ['input', 'scenario', 'seed']

    @classmethod
    def from_path(cls, path, base):
        relpath = os.path.relpath(path, base)
        scenario_and_seed, _ = os.path.splitext(relpath)
        scenario, seed = os.path.splitext(scenario_and_seed)

        if seed == '':
            seed = None
            scenario = None
        else:
            seed = seed.lstrip('.')

        return cls(
            path,
            relpath,
            scenario = scenario,
            seed = seed,
        )


def generate_inputs_single_file(path):
    yield InputInfo.from_path(
        path,
        os.path.dirname(path),
    )

def generate_inputs(directory, extension='.json'):
    with os.scandir(directory) as entries:
        for entry in entries:
            if (
                os.path.splitext(entry.name)[1] == extension
                and entry.is_file()
            ):
                yield InputInfo.from_path(
                    os.path.join(directory, entry.name),
                    directory,
                )

def generate_inputs_recursively(directory, extension='.json'):
    for root, _, files in os.walk(directory):
        for file in files:
            if os.path.splitext(file)[1] == extension:
                yield InputInfo.from_path(os.path.join(root, file), directory)

def repeat_each(it, n=None):
    return itertools.chain.from_iterable(
        itertools.repeat(item, n) for item in it
    )


parser = argparse.ArgumentParser(
    description = 'Performs benchmarks using multiple inputs and outputs the '
                  'resulting run time',
)
parser.add_argument(
    'executable',
    type = str,
    help = 'Path to the generator configuration to be used. '
           'If the special value "-" is passed, the configuration is read '
           'from stdin.',
)
parser.add_argument(
    'input_path',
    metavar = 'input-path',
    type = str,
    help = 'Path to the generator configuration to be used. '
           'If the special value "-" is passed, the configuration is read '
           'from stdin.',
)
parser.add_argument(
    '--recursive',
    action = 'store_true',
    help = 'If set, the input directory is searched recursively.',
)
parser.add_argument(
    '--repeat', '-r',
    type = int,
    default = 1,
    help = 'The number of executions per input. (default 1)',
)
parser.add_argument(
    '--row-prefix',
    type = str,
    help = 'The row prefix is prepended to each row in the output. '
           'The prefix is interpreted as csv.',
)
parser.add_argument(
    '--header-prefix',
    type = str,
    help = 'The header prefix is prepended to the header row in the output. '
           'The prefix is interpreted as csv.',
)
parser.add_argument(
    '--header-only',
    action = 'store_true',
    help = 'Seed to be used for the pseudo-random number generator. '
           'The value is expected in hex representation. '
           'If omitted, a seed is generated from a suitable source.',
)
parser.add_argument(
    '--no-header',
    action = 'store_true',
    help = 'If set, the header row is omitted from output. ',
)


def main():
    args = parser.parse_args()

    if args.row_prefix is not None:
        row_prefix = listify_csv_str(args.row_prefix)
    else:
        row_prefix = []

    if args.header_prefix is not None:
        header_prefix = listify_csv_str(args.header_prefix)
    else:
        header_prefix = []

    writer = csv.writer(sys.stdout)

    if args.header_only or not args.no_header:
        write_csv_header(writer, header_prefix=header_prefix)
    if args.header_only:
        return

    if os.path.isfile(args.input_path):
        inputs = generate_inputs_single_file(args.input_path)
    elif args.recursive:
        inputs = generate_inputs_recursively(args.input_path)
    else:
        inputs = generate_inputs(args.input_path)

    if args.repeat > 1:
        inputs = repeat_each(inputs, args.repeat)

    benchmarker = Benchmarker(args.executable, inputs)

    for result in benchmarker:
        write_csv_row(writer, result, row_prefix=row_prefix)

def listify_csv_str(csv_str):
    cst_str_io = io.StringIO(csv_str)

    reader = csv.reader(cst_str_io)
    return list(row for row in reader)[0]

def write_csv_header(writer, header_prefix=[]):
    writer.writerow(header_prefix + InputInfo.csv_header() + ["run_time"])

def write_csv_row(writer, result, row_prefix=[]):
    writer.writerow(row_prefix + result.input.csv_info() + [result.run_time])

if __name__ == '__main__':
    main()
