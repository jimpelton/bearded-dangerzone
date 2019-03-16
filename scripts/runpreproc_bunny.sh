#!/bin/sh

d=$1
outDir=$2
tfd=$3

for i in {1,16,24,32,48,64,72,96}; do
    NUMBA_WARNINGS=1 python ../preproc/src/pyproc.py \
        --raw ${d}/bunny_4096x4096x2888_ushort.raw \
        --out ${outDir}/bunny-4k-${i}x${i}x${i}-zero_to_one.json \
        --bx ${i} --by ${i} --bz ${i} \
        --vx 4096 --vy 4096 --vz 2888 \
        --tf ${tfd}/zero_to_one.otf \
        --dtype u1 | tee ${outDir}/bunny-4k-${i}x${i}x${i}-zero_to_one.txt
done

for i in {1,16,24,32,48,64,72,96}; do
    NUMBA_WARNINGS=1 python ../preproc/src/pyproc.py \
        --raw ${d}/bunny_4096x4096x2888_ushort.raw \
        --out ${outDir}/bunny-4k-${i}x${i}x${i}-bunny.json \
        --bx ${i} --by ${i} --bz ${i} \
        --vx 4096 --vy 4096 --vz 2888 \
        --tf ${tfd}/bunny_512x512x361.otf \
        --dtype u1 | tee ${outDir}/bunny-4k-${i}x${i}x${i}-bunny.txt
done


