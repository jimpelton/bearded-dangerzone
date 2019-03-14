#!/bin/sh

d=$1
outDir=$2
tfd=$3

for i in {1,16,24,32,48,64,72,96}; do
    NUMBA_WARNINGS=1 python ../preproc/src/pyproc.py \
        --raw ${d}/ \
        --out ${outDir}/hop_4k_${i}x${i}x${i}.json \
        --bx ${i} --by ${i} --bz ${i} \
        --vx 3509 --vy 3787 --vz 4096 \
        --tf ${tfd}/zero_to_one.otf \
        --dtype u1 | tee ${outDir}/hop-4k-${i}x${i}x${i}-zero_to_one.txt
done

for i in {1,16,24,32,48,64,72,96}; do
    NUMBA_WARNINGS=1 python ../preproc/src/pyproc.py \
        --raw ${d}/Hop_Flower-Resampled-3509x3787x4096.raw \
        --out ${outDir}/hop_4k_${i}x${i}x${i}.json \
        --bx ${i} --by ${i} --bz ${i} \
        --vx 3509 --vy 3787 --vz 4096 \
        --tf ${tfd}/hop_default.otf \
        --dtype u1 | tee ${outDir}/hop-4k-${i}x${i}x${i}-hop_default.txt
done


