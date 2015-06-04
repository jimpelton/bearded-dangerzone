__author__ = 'jim'

import os
import sys
import argparse

row_names = [
    'IA Bottleneck',
    'IA SOL',
    'Primitive Setup Bottleneck',
    'Primitive Setup SOL',
    'ROP Bottleneck',
    'ROP SOL',
    'Rasterization Bottleneck',
    'Rasterization SOL',
    'SHD Bottleneck',
    'SHD SOL',
    'TEX Bottleneck',
    'TEX SOL',
    'FB Bottleneck',
    'FB SOL',
    'L2 Bottleneck',
    'tex_cache_hitrate',
    'l2_read_bytes_mem',
    'l2_read_bytes_tex',
    'shd_tex_read_bytes',
    'shd_tex_requests',
    'inst_executed_ps',
    'inst_executed_ps_ratio',
    'inst_executed_vs',
    'inst_executed_vs_ratio',
    'setup_primitive_count',
    'shaded_pixel_count'
]


def value(line):
    line_split = str(line).split(' ')
    try:
        value_col_idx = line_split.index('value:')
    except:
        print("'value:' was not found in line.")
        return

    if len(line_split) < value_col_idx + 1:
        return -1.0

    val_str = line_split[value_col_idx + 1]
    rval = -1.0
    try:
        rval = float(val_str)
    except:
        print('value was not a float: {}'.format(val_str))

    return rval


def parse_file(path):
    counters = []
    with os.open(path, 'r') as f:
        for line in f.readlines():
            counter = [x for x in row_names if line.startswith(x)][0]
            v = value(line)
            if v > -1.0:
                counters.append((counter, v))

    return counters


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('-f', help='Directory with files to parse', type=str)
    parser.add_argument('-o', help='Output directory', type=str)

    args = parser.parse_args(args=sys.argv[1:])
    return args


def main():
    args = parse_args()

    if not os.path.exists(args.f):
        print('Path {} does not exist'.format(args.f))

    files = os.listdir(args.f)

    files_counters = dict()
    for p in files:
        files_counters[p] = parse_file(p)

    return


if __name__ == '__main__':
    main()
