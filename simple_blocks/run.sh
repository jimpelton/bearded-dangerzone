#!/bin/sh 

./simple_blocks \
    --main-mem 2G \
    --gpu-mem 1G \
    --screen-height 1280 \
    --screen-width 1024 \
    -s 512 \
    --index-file /home/jim/bunny_512_2x2x2.json \
    --otf ~/james.s.pelton@gmail.com/school/grad_school/JimPelton_Project/transfer_functions/otf/bunny_512x512x361.otf \
    --ctf ~/james.s.pelton@gmail.com/school/grad_school/JimPelton_Project/transfer_functions/ctf/hop_default.ctf \
    -f /run/media/jim/New\ Volume/bunny/bunny_512x512x361.little-endian.raw
