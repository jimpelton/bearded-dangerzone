#!/bin/sh

d=$1
outDir=$2
tfd=$3

for i in {1,16,24,32,48,64,72,96}; do
    NUMBA_WARNINGS=1 python ../preproc/src/pyproc.py \
        --raw ${d}/oa2-2391x3084x2452-uchar.raw \
        --out ${outDir}/oa2-4k-${i}x${i}x${i}-zero_to_one.json \
        --bx ${i} --by ${i} --bz ${i} \
        --vx 2391 --vy 3084 --vz 2452 \
        --tf ${tfd}/zero_to_one.otf \
        --dtype u1 | tee ${outDir}/oa2-4k-${i}x${i}x${i}-zero_to_one.txt
done

for i in {1,16,24,32,48,64,72,96}; do
    NUMBA_WARNINGS=1 python ../preproc/src/pyproc.py \
        --raw ${d}/oa2-2391x3084x2452-uchar.raw \
        --out ${outDir}/oa2-4k-${i}x${i}x${i}-oa2.json \
        --bx ${i} --by ${i} --bz ${i} \
        --vx 2391 --vy 3084 --vz 2452 \
        --tf ${tfd}/oa2-478x616x490-uchar.otf \
        --dtype u1 | tee ${outDir}/oa2-4k-${i}x${i}x${i}-oa2.txt
done


