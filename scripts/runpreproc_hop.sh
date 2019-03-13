#!/bin/sh

for i in {1,16,24,32,48,64,72,96}; do
    NUMBA_WARNINGS=1 python preproc/src/pyproc.py \
        --raw /run/media/jim/data/volumes/INL/Josh_Kane/hop_flower/Hop_Flower-Resampled-3509x3787x4096.raw \
        --out ../sample_volumes/hop_4k_${i}x${i}x${i}.json \
        --bx ${i} --by ${i} --bz ${i} \
        --vx 3509 --vy 3787 --vz 4096 \
        --tf /home/jim/james.s.pelton@gmail.com/school/grad_school/JimPelton_Project/transfer_functions/otf/hop_default.otf \
        --dtype u1
done


