# Tools
AS=nasm
LD=ld

# Flags
ASFLAGS=-f elf32
BINARYASFLAGS=-f bin

LDFLAGS=-Tlink.ld --oformat binary

# Deps
STAGE1_DEPS=boot/x86/stage1/stage1.asm
STAGE15_DEPS=boot/x86/stage1/stage1_5/stage1_5.asm
STAGE2_DEPS=boot/x86/stage2/stage2.asm

# Images
BIN1=build/stage1.bin
BIN15=build/stage1_5.bin
GEBL=build/goldeneaglebl.bin

# Object files
STAGE1_5=boot/x86/stage1/stage1_5/stage1_5.o
STAGE2=boot/x86/stage2/stage2.o

.PHONY: all
all: $(GEBL)

.PHONY: clean
clean:
	rm boot/x86/*/*/*.o
	rm boot/x86/stage2/*.o
	rm build/*.bin

$(STAGE1_5): $(STAGE15_DEPS)
	$(AS) $(ASFLAGS) -o $(STAGE1_5) $(STAGE15_DEPS)

$(STAGE2): $(STAGE2_DEPS)
	$(AS) $(ASFLAGS) -o $(STAGE2) $(STAGE2_DEPS)

$(BIN1): $(STAGE1_DEPS)
	$(AS) $(BINARYASFLAGS) -o $(BIN1) $(STAGE1_DEPS)

$(BIN15): $(STAGE1_5) $(STAGE2)
	$(LD) $(LDFLAGS) -o $(BIN15) $(STAGE1_5) $(STAGE2)

$(GEBL): $(BIN1) $(BIN15)
	dd if=$(BIN1) of=$(GEBL) seek=0
	dd if=$(BIN15) of=$(GEBL) ibs=512 seek=1 conv=sync