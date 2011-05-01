# Tools
AS=nasm
LD=ld
CC=gcc

# Flags
ASFLAGS=-f elf32
BINARYASFLAGS=-f bin

CCFLAGS=-c -m32 -nostdlib -e kmain -nodefaultlibs

LDFLAGS=-Tlink.ld --oformat binary -melf_i386

# Deps
STAGE1_DEPS=boot/x86/stage1/stage1.asm
STAGE15_DEPS=boot/x86/stage1/stage1_5/stage1_5.asm
STAGE2_DEPS=boot/x86/stage2/stage2.asm
KERN_DEPS=kern/kmain.c

# Images
BIN1=build/stage1.bin
BIN2=build/stage1_5.bin
GEBL=build/goldeneaglebl.bin

# Object files
STAGE1_5=boot/x86/stage1/stage1_5/stage1_5.o
STAGE2=boot/x86/stage2/stage2.o
KERN=kern/kmain.o

.PHONY: all
all: $(GEBL)

.PHONY: test
test: all
	bochs -f bochsrc

.PHONY: clean
clean:
	rm boot/x86/*/*/*.o
	rm boot/x86/stage2/*.o
	rm kern/*.o
	rm build/*.bin

$(STAGE1_5): $(STAGE15_DEPS)
	$(AS) $(ASFLAGS) -o $(STAGE1_5) $(STAGE15_DEPS)

$(STAGE2): $(STAGE2_DEPS)
	$(AS) $(ASFLAGS) -o $(STAGE2) $(STAGE2_DEPS)

$(KERN): $(KERN_DEPS)
	$(CC) $(CCFLAGS) -o $(KERN) $(KERN_DEPS)

$(BIN1): $(STAGE1_DEPS)
	$(AS) $(BINARYASFLAGS) -o $(BIN1) $(STAGE1_DEPS)

$(BIN2): $(STAGE1_5) $(STAGE2) $(KERN)
	$(LD) $(LDFLAGS) -o $(BIN2) $(STAGE1_5) $(STAGE2) $(KERN)

$(GEBL): $(BIN1) $(BIN2)
	dd if=$(BIN1) of=$(GEBL) seek=0
	dd if=$(BIN2) of=$(GEBL) seek=1 ibs=512 conv=sync
