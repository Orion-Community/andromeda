include make/Makedeps

# Tools
AS=nasm
LD=ld
CC=gcc

# Flags
ASFLAGS=-f elf32
BINARYASFLAGS=-f bin
CCFLAGS=-c -m32 -nostdlib -e kmain -nodefaultlibs -fno-stack-protector -fno-builtin -nostdinc -Iinclude
LDFLAGS=-Tlink.ld --oformat binary -melf_i386

# Deps
STAGE1_DEPS=boot/x86/stage1/stage1.asm
MBR=boot/x86/masterboot.asm

# Images
MBR_IMG=build/masterboot.bin
BIN1=build/stage1.bin
BIN2=build/stage15.bin
GEBL=build/goldeneaglebl.bin



.PHONY: all
all: $(GEBL)

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
	$(AS) $(BINARYASFLAGS) -D __HDD -o $(MBR_IMG) $(MBR)

$(BIN1): $(STAGE1_DEPS)
	$(AS) $(BINARYASFLAGS) -o $(BIN1) $(STAGE1_DEPS)

$(BIN2): $(DEPS)
	$(LD) $(LDFLAGS) -o $(BIN2) $(DEPS)

$(GEBL): $(MBR_IMG) $(BIN1) $(BIN2)
	dd if=$(MBR_IMG) of=$(GEBL) seek=0
	dd if=$(BIN1) of=$(GEBL) seek=1
	dd if=$(BIN2) of=$(GEBL) seek=2 ibs=512 conv=sync
