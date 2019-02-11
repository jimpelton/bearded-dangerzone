#!/bin/sh 

./simple_blocks \
    --main-mem 2G \
    --gpu-mem 512M \
    --screen-height 800 \
    --screen-width 600 \
    --index-file ../bunny_512_1x1x1.json \
    --otf ../sample_volumes/bunny_512x512x361.otf \
    --ctf ../sample_volumes/hop_default.ctf \
    -f ../sample_volumes/bunny_512x512x361.raw \
    --smod 512
