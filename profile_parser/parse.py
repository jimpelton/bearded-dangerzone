__author__ = 'jimpelton'

import sys
import csv
import os.path

hud_frame_time_usec = int(0)
header = []
column_indices = {}

column_values = {
    'gpu_idle_cycles': 0,
    'L2 Bottleneck': float(0),
    'shd_tex_read_bytes': 0,
    'l2_read_bytes_tex': 0,
    'l2_read_bytes_mem': 0
}


def main(args):
    csvpath = ''

    if len(args) > 1:
        csvpath = args[1]
    else:
        print('Provide a path to profile values.')
        exit(1)

    with open(csvpath, 'r', newline='') as csvfile:
        reader = csv.reader(csvfile)
        rows = list(reader)
        hud_frame_time_usec = int(rows[0][1])
        header_row = rows[1]

        print(hud_frame_time_usec)
        print(header_row)

        gpu_idle_cycles_index = header_row.index('gpu_idle') + 1
        column_indices['gpu_idle_cycles'] = gpu_idle_cycles_index
        column_indices['L2 Bottleneck'] = header_row.index('L2 Bottleneck')
        column_indices['shd_tex_read_bytes'] = header_row.index('shd_tex_read_bytes')
        column_indices['l2_read_bytes_tex'] = header_row.index('l2_read_bytes_tex')
        column_indices['l2_read_bytes_mem'] = header_row.index('l2_read_bytes_mem')
        highest = max(column_indices.values())

        row_count = int(0)
        for row in rows[2:]:
            if len(row) < highest:
                continue
            row_count += 1

            column_values['gpu_idle_cycles'] += float(row[column_indices['gpu_idle_cycles']])
            column_values['L2 Bottleneck'] += float(row[column_indices['L2 Bottleneck']])
            column_values['shd_tex_read_bytes'] += float(row[column_indices['shd_tex_read_bytes']])
            column_values['l2_read_bytes_tex'] += float(row[column_indices['l2_read_bytes_tex']])
            column_values['l2_read_bytes_mem'] += float(row[column_indices['l2_read_bytes_mem']])

        for k in column_values:
            column_values[k] //= row_count

    print("File name: {}".format(os.path.split(csvpath)[-1]))
    print("Averages over {} frames:".format(row_count))
    for k, v in column_values.items():
        print("\t{}: {}".format(k, v))

    return


if __name__ == '__main__':
    main(sys.argv)