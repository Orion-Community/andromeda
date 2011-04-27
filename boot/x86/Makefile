AS=nasm
CC=gcc
LD=ld

# Bootloader
ASFLAGS=-f elf32
STAGE1=stage1/goldeneagle.asm
STAGE1DEPS=print.asm stage1/imageloader.asm stage2/A20.asm stage2/fenixloader.asm stage2/GDT.asm stage2/kmain.c stage2/pm.asm stage2/pmodemain.asm

#Kernel
#KERNDEPS=stage2/kmain.c
#KERN=stage2/kmain.c
#CCFLAGS= -o kernel.o -O -c -Wall -Werror -nostdlib -nostartfiles -nodefaultlibs

# linker
LDFLAGS=-oformat=elf32-i386 -melf_i386 -Tlink.ld  

# Files
DISK=./goldeneaglebl.bin
BLOADER=./loader.bin
GOLDENEAGLE=./goldeneagle.bin

.PHONY: all
all: goldeneaglebl.bin

.PHONY: test
test: all
	bochs -f bochsrc

.PHONY: clean
clean:
	rm *.bin

loader.bin: $(STAGE1) $(STAGE1DEPS)
	$(AS) $(ASFLAGS) $(STAGE1) -o $(BLOADER)

goldeneagle.bin: loader.bin
	$(LD) $(LDFLAGS) -o $(GOLDENEAGLE)

goldeneaglebl.bin: goldeneagle.bin
	dd if=$(BIN1) of=$(DISK) conv=sync ibs=512
	rm $(BIN1)
