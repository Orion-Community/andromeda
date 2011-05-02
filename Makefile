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

# Images
BIN1=build/stage1.bin
BIN2=build/stage15.bin
GEBL=build/goldeneaglebl.bin



.PHONY: all
all: $(GEBL)

.PHONY: test
test: all
	bochs -f bochsrc

.PHONY: clean
clean:
	rm *.o
	rm build/*.bin

$(BIN1): $(STAGE1_DEPS)
	$(AS) $(BINARYASFLAGS) -o $(BIN1) $(STAGE1_DEPS)

$(BIN2): $(DEPS)
	$(LD) $(LDFLAGS) -o $(BIN2) $(DEPS)

$(GEBL): $(BIN1) $(BIN2)
	dd if=$(BIN1) of=$(GEBL) seek=0
	dd if=$(BIN2) of=$(GEBL) seek=1 ibs=512 conv=sync
