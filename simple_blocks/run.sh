#!/bin/sh 

./simple_blocks \
    --main-mem 2G \
    --gpu-mem 512M \
    --screen-height 800 \
    --screen-width 600 \
    -s 512 \
    --index-file ~/bunny_512_1x1x1.json \
    --otf ~/Downloads/transfer_functions/otf/bunny_512x512x361.otf \
    --ctf ~/Downloads/transfer_functions/ctf/hop_default.ctf \
    -f /run/media/jim/New\ Volume/bunny/bunny_512x512x361.little-endian.raw
