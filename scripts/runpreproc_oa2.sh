#!/bin/sh

for i in {1,16,24,32,48,64,72,96}; do
    NUMBA_WARNINGS=1 python preproc/src/pyproc.py \
        --raw /run/media/jim/data/volumes/oxford/oa2/oa2_raw/oa2-2391x3084x2452-uchar.raw \
        --out ../sample_volumes/oa2_4k_${i}x${i}x${i}.json \
        --bx ${i} --by ${i} --bz ${i} \
        --vx 2391 --vy 3084 --vz 2452 \
        --tf /home/jim/james.s.pelton@gmail.com/school/grad_school/JimPelton_Project/transfer_functions/otf/oa2-478x616x490-uchar.otf \
        --dtype u1
done


