include make/Makedeps

# Tools
AS=nasm
LD=ld
CC=gcc

# Flags
ASFLAGS=-f elf32 -D __$(BUILD_TARGET) -D __DEBUG -I"include/"
BINARYASFLAGS=-f bin -D __$(BUILD_TARGET) -D __DEBUG -I"include/"
CCFLAGS=-c -m32 -nostdlib -e kmain -nodefaultlibs -fno-stack-protector -fno-builtin -nostdinc -Iinclude -D __DEBUG
LDFLAGS=-Tlink.ld --oformat binary -melf_i386
BUILD_TARGET=HDD

# Deps
STAGE1_DEPS=boot/x86/stage1/stage1.asm
MBR=boot/x86/masterboot.asm

# Images
MBR_IMG=build/masterboot.bin
BIN1=build/stage1.bin
BIN2=build/stage15.bin
OL=build/openloader.bin
BOOTBLOCK=build/bootblock.bin



.PHONY: all
all: $(OL)

.PHONY: masterboot
masterboot: $(MBR_IMG)
	@echo "CAUTION: The mbr is still highly expirimental!"

.PHONY: test
test: all
	bochs -f bochsrc

.PHONY: ctest
ctest: clean all test

.PHONY: clean
clean:
	rm *.o
	rm build/*.bin

$(MBR_IMG): $(MBR)
	$(AS) $(BINARYASFLAGS) -o $(MBR_IMG) $(MBR)

$(BIN1): $(STAGE1_DEPS)
	$(AS) $(BINARYASFLAGS) -o $(BIN1) $(STAGE1_DEPS)

$(BIN2): $(DEPS)
	$(LD) $(LDFLAGS) -o $(BIN2) $(DEPS)

$(BOOTBLOCK): $(BIN1) $(BIN2)
	dd if=$(BIN1) of=$(BOOTBLOCK) seek=0
	dd if=$(BIN2) of=$(BOOTBLOCK) seek=1 ibs=512 conv=sync

$(OL): $(MBR_IMG) $(BOOTBLOCK)
	dd if=$(MBR_IMG) of=$(OL) seek=0
	dd if=$(BOOTBLOCK) of=$(OL) seek=2048
