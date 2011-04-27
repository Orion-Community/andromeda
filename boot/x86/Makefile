AS=nasm
CC=gcc

# Bootloader
ASFLAGS=-f bin -o goldeneagle.bin
STAGE1=stage1/goldeneagle.asm
STAGE1DEPS=print.asm stage1/imageloader.asm stage2/A20.asm stage2/fenixloader.asm stage2/GDT.asm stage2/kmain.c stage2/pm.asm stage2/pmodemain.asm

#Kernel
KERNDEPS=stage2/kmain.c
KERN=stage2/kmain.c
CCFLAGS= -o kernel.o -O -c -Wall -Werror -nostdlib -nostartfiles -nodefaultlibs

# Files
DISK=./goldeneaglebl.bin
BIN1=./goldeneagle.bin

.PHONY: all
all: goldeneaglebl.bin

.PHONY: test
test: all
	bochs -f bochsrc

.PHONY: clean
clean:
	rm *.bin

goldeneagle.bin: $(STAGE1) $(STAGE1DEPS)
	$(AS) $(ASFLAGS) $(STAGE1) -o $(BIN1)

kernel.o: $(KERNDEPS)
	$(CC) $(KERN) $(CCFLAGS)

goldeneaglebl.bin: goldeneagle.bin kernel.o
	dd if=$(BIN1) of=$(DISK) conv=sync ibs=512
	rm $(BIN1)
