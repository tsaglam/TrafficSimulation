
import json
import os
import argparse
import sys


parser = argparse.ArgumentParser(
    description = 'Creates copies of a simulator input file with reduced '
                  'numbers of time_steps values at a regular interval.',
)
parser.add_argument(
    'input_path',
    metavar = 'input-path',
    type = str,
    help = 'Path to the input file to be used as the basis for output files. '
           'If the special value "-" is passed, the input file is read from '
           'stdin.',
)
parser.add_argument(
    '--interval', '-i',
    type = int,
    default = 1,
    help = 'Width specifies the interval of the emitted output files. That '
           'is the number of time steps between two emitted output files '
           '(default 1).',
)
parser.add_argument(
    '--output-path',
    help = 'Path to which output files are written. '
           'The number of written files is determined by the time_steps '
           'value in the input file as well as the --interval. '
           'If this is not specified, the input path is used and ".t_{t}" is '
           'inserted before the file extension'
           'Variables available for substitution: '
           '{t} the time_steps value for the output file.',
)


def main():
    args = parser.parse_args()

    if args.output_path is not None and "{t}" not in args.output_path:
        print(
            "The output path must contain the variable {t}.",
            file = sys.stderr,
        )
        sys.exit(1)

    if args.output_path is None:
        base, ext = os.path.splitext(args.input_path)
        args.output_path = base + ".t_{t}" + ext

    if args.input_path == "-":
        input_dict = json.load(sys.stdin)
    else:
        with open(args.input_path) as f:
            input_dict = json.load(f)

    max_time_steps = input_dict["time_steps"]

    for t_sub in range(0, max_time_steps):
        time_steps = max_time_steps - t_sub

        output_path = args.output_path.format(t=time_steps)

        input_dict["time_steps"] = time_steps

        with open(output_path, 'w') as f:
            json.dump(input_dict, f)


if __name__ == '__main__':
    main()
