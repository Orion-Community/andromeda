#!/bin/bash

nasm -f bin -o stage1/stage1.bin stage1/goldeneagle.asm
nasm -f bin -o stage2/stage2.bin stage2/fenixloader.asm

sleep 1

# Copy first sector
dd if=stage1/stage1.bin of=goldeneaglebl.bin seek=0

# Copy second sector
dd if=stage2/stage2.bin of=goldeneaglebl.bin seek=1
