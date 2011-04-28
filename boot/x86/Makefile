AS=nasm
LD=ld
CC=gcc

# AS flags
ASFLAGS=-f elf32

# LD flags
LDFLAGS=-Tlink.ld --oformat binary

# CC flags
CCFLAGS=-c -e kmain

# src files
STAGE1=stage1/goldeneagle.asm
KMAIN=stage2/kmain.c

# Deps
STAGE1_DEPS=print.asm stage1/imageloader.asm stage2/A20.asm stage2/fenixloader.asm stage2/GDT.asm stage2/pm.asm stage2/pmodemain.asm
KERN_DEPS=stage2/kmain.c

# Image's
DISK=./goldeneaglebl.bin
TEMP=./temp.bin
LOADER=./loader.o
KERN=kern.o

.PHONY: all
all: goldeneaglebl.bin

.PHONY: test
test: all
	bochs -f bochsrc

.PHONY: clean
clean:
	rm *.o
	rm *.bin

kern.o: $(KERN_DEPS)
	$(CC) $(CCFLAGS) -o $(KERN) $(KMAIN)

loader.o:$(STAGE1) $(STAGE1_DEPS)
	$(AS) $(ASFLAGS) -o $(LOADER) $(STAGE1)

goldeneaglebl.bin: $(LOADER) $(KERN)
	$(LD) $(LDFLAGS) -o $(TEMP) $(LOADER) $(KERN)
	dd if=$(TEMP) of=$(DISK) conv=sync ibs=512
