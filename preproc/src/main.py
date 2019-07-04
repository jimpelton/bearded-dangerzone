import time
import argparse
import sys
import os
import numpy as np
import numba
from numba import jit, njit, autojit, config, threading_layer

import indexfile
import volume

#config.THREADING_LAYER = 'tbb'

def parse_args(args):
    parser = argparse.ArgumentParser()
    parser.add_argument("--raw", default="", help="Input file")
    parser.add_argument("--out", default="", help="Output file")

    parser.add_argument("--bx", default=1, type=int, help="Blocks along x-dim")
    parser.add_argument("--by", default=1, type=int, help="Blocks along y-dim")
    parser.add_argument("--bz", default=1, type=int, help="Blocks along z-dim")

    parser.add_argument("--vx", default=1, type=int, help="Vol dims X")
    parser.add_argument("--vy", default=1, type=int, help="Vol dims Y")
    parser.add_argument("--vz", default=1, type=int, help="Vol dims Z")

    parser.add_argument("--dtype", default="", type=str)

    parser.add_argument("--tf", default='', type=str, help="Transfer function")

    return parser.parse_args(args)


@njit(fastmath=True)
def volume_analysis_jit(raw, num_vox):
    """Compute the min and max and the total for the volume and return them in a tuple.

       These values are needed for the voxel analysis done later on. The returned tuple is
       in the order: (min, max, total).
    """
    mn = np.iinfo(raw.dtype).max
    mx = -np.iinfo(raw.dtype).min
    acc = numba.float64(0.0)
    for i in numba.prange(num_vox):
        x = raw[i]
        if x < mn:
            mn = x
        if x > mx:
            mx = x

        acc += x

    return (mn, mx, acc)


def run_volume(raw, num_vox):
    """Run and time the volume min, max and total computation.
    """
    start = time.time()
    vol_min, vol_max, vol_tot = volume_analysis_jit(raw, num_vox)
    vol_end = time.time()
    vol_time = vol_end - start

    print(f"Volme level elapsed time: {vol_time}")
    return vol_min, vol_max, vol_tot


@njit(fastmath=True, parallel=True)
def block_analysis_jit(raw, xp, yp,
                     vmin: np.float64, vmax: np.float64,
                     vdims: np.ndarray, bdims: np.ndarray,
                     bcount: np.ndarray,
                     blocks: np.ndarray):
    diff = vmax - vmin
    num_vox = np.prod(vdims)

    for i in numba.prange(num_vox):
        bI = numba.uint64((i % vdims[0]) / bdims[0])
        bJ = numba.uint64(((i / vdims[0]) % vdims[1]) / bdims[1])
        bK = numba.uint64(((i / vdims[0]) / vdims[1]) / bdims[2])

        if bI < bcount[0] and bJ < bcount[1] and bK < bcount[2]:
            # Normalize the voxel value to [-1 .. 1]
            x = numba.float64((raw[i] - vmin) / diff)

            max_idx = len(xp) - 1

            idx = int((x * max_idx) + 0.5)
            if idx > max_idx:
                k0 = int(max_idx - 1)
                k1 = int(max_idx)
            elif idx == 0:
                k0 = int(0)
                k1 = int(1)
            else:
                k0 = int(idx - 1)
                k1 = int(idx)

            # Compute relevance value by linear interpolation of the transfer funciton.
            d = (x - xp[k0]) / (xp[k1] - xp[k0])
            rel = numba.float64(yp[k0] * (1.0 - d) + yp[k1] * d)

            # Compute the 1D index of the block and add the relevance value to the existing
            # relevance value for the block.
            bIdx = bI + bcount[0] * (bJ + bK * bcount[1])
            blocks[bIdx] += rel


def run_block(raw,
        xp: np.ndarray,
        yp: np.ndarray,
        vmin: np.float64,
        vmax: np.float64,
        vdims: np.ndarray,
        bdims: np.ndarray,
        bcount: np.ndarray):
    """Run the block level analysis and return the relevancies as a list of np.float64
    """
    blocks = np.zeros(np.prod(bcount), dtype=np.float64)
    start = time.time()
    block_analysis_jit(raw, xp, yp, vmin, vmax, vdims, bdims, bcount, blocks)
    volend = time.time()
    vol_time = volend - start
    print(f"Block level time: {vol_time}")

    block_vox_count = np.prod(bdims)
    for i in range(len(blocks)):
        blocks[i] = blocks[i] / block_vox_count

    return blocks

def main():
    cargs = parse_args(sys.argv[1:])

    data_type = np.dtype(cargs.dtype)
    raw = np.memmap(cargs.raw, dtype=data_type, mode='r')

    tf_x = np.loadtxt(cargs.tf, dtype=np.float64, usecols=0, skiprows=1)
    tf_y = np.loadtxt(cargs.tf, dtype=np.float64, usecols=1, skiprows=1)
    vdims = np.array([cargs.vx, cargs.vy, cargs.vz], dtype=np.uint64)
    bcount = np.array([cargs.bx, cargs.by, cargs.bz], dtype=np.uint64)
    bdims = np.divide(vdims, bcount)
    blocks = np.zeros(cargs.bx * cargs.by * cargs.bz)
    block_size = vdims / bcount
    block_extent = block_size * bcount
    print("\nRunning for {} blocks".format(bcount))
    print('Running volume analysis')
    vol_min, vol_max, vol_tot = run_volume(raw, np.prod(vdims))

    print('Running relevance analysis')
    relevancies = run_block(raw, tf_x, tf_y, vol_min, vol_max, vdims, bdims, bcount)
    rov_min = np.min(relevancies)
    rov_max = np.max(relevancies)

    print("Creating index file")
    vol_path, vol_name = os.path.split(cargs.raw)
    tr_path, tr_name = os.path.split(cargs.tf)

    max_dim = np.max(vdims)
    world_dims = [ vdims[0]/max_dim, vdims[1]/max_dim, vdims[2]/max_dim]

    vol_stats = volume.VolStats(min=vol_min, max=vol_max, avg=0.0, tot=vol_tot)
    vol = volume.Volume(world_dims, vdims.tolist(), rov_min, rov_max)

    idx_start = time.time()
    blocks = indexfile.create_file_blocks(bcount, raw.dtype, vol, relevancies)

    ifile = indexfile.IndexFile(**{
        'world_dims': world_dims,
        'vol_stats': vol_stats,
        'vol_name': vol_name,
        'vol_path': vol_path,
        'volume': vol,
        'tr_func': tr_name,
        'dtype': raw.dtype.name,
        'num_blocks': bcount,
        'blocks_extent': block_extent,
        'blocks': blocks,
        })
    ifile.write(cargs.out)
    idx_end = time.time()
    index_time = idx_end - idx_start
    print(f"Index file time: {index_time}")

if __name__ == '__main__':
    main()


