#!/bin/bash

nasm -f bin -o boot/x86/stage1/stage1.bin boot/x86/stage1/stage1.asm
nasm -f elf -o boot/x86/stage1/stage1_5/stage1_5.o boot/x86/stage1/stage1_5/stage1_5.asm
ld -Tlink.ld --oformat binary -o boot/x86/stage1/stage1_5/stage1_5.bin boot/x86/stage1/stage1_5/stage1_5.o

sleep 1

# Copy first sector
dd if=boot/x86/stage1/stage1.bin of=goldeneaglebl.bin seek=0

# Copy second sector
dd if=boot/x86/stage1/stage1_5/stage1_5.bin of=goldeneaglebl.bin seek=1 ibs=512 conv=sync