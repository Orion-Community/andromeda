# Tools
AS=nasm
LD=ld
CC=gcc
MAKE=make

# Flags
ASFLAGS=-felf32 -I"include/" $(FLAGS)
BINARYASFLAGS=-f bin -I"include/" $(FLAGS)
CCFLAGS=-c -m32 -nostdlib -nodefaultlibs -fno-stack-protector -fno-builtin -nostdinc -Iinclude -e kmain $(FLAGS)
LDFLAGS=-Tlink.ld --oformat binary -melf_i386
FLAGS=-D __HDD -D __DEBUG

# Deps
BOOTBLOCK_DEPS=boot.o kern.o arch.o mm.o sys.o video.o error.o

# Images
MBR_IMG=build/masterboot.bin
BIN1=build/stage1.bin
BIN2=build/stage15.bin
OL=build/openloader.bin
BOOTBLOCK=bootblock.bin



.PHONY: all
all: x86
	@echo "Finished building the openLoader core images. You can find them in src/build."

.PHONY: x86
x86: $(BOOTBLOCK)

$(BOOTBLOCK):
	$(MAKE) -C boot x86
	$(MAKE) -C arch x86
	$(MAKE) -C kern
	$(MAKE) -C error
	$(MAKE) -C sys
	$(MAKE) -C mm
	$(MAKE) -C video
	mv -v boot/boot.o ./
	mv -v boot/masterboot.bin ./
	mv -v boot/stage1.bin ./
	mv -v arch/arch.o ./
	mv -v sys/sys.o ./
	mv -v error/error.o ./
	mv -v mm/mm.o ./
	mv -v kern/kern.o ./
	mv -v video/video.o ./
	$(LD) $(LDFLAGS) *.o -o $(BOOTBLOCK)


.PHONY: clean
clean:
	$(MAKE) -C boot clean
	$(MAKE) -C arch clean
	$(MAKE) -C kern clean
	$(MAKE) -C error clean
	$(MAKE) -C sys clean
	$(MAKE) -C mm clean
	$(MAKE) -C video clean

#$(BOOTBLOCK): $(BIN1) $(BIN2)
#	dd if=$(BIN1) of=$(BOOTBLOCK) seek=0
#	dd if=$(BIN2) of=$(BOOTBLOCK) seek=1 ibs=512 conv=sync

#$(OL): $(MBR_IMG) $(BOOTBLOCK)
#	dd if=$(MBR_IMG) of=$(OL) seek=0
#	dd if=$(BOOTBLOCK) of=$(OL) seek=2048

