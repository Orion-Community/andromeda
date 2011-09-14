# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../../../include/" -I"../interface/" $(FLAGS)
BINASFLAGS=-fbin -I"../../../include/" -I"../interface/" $(FLAGS)
MAKE=make
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386
LD=ld
DD=dd

# targets

STAGE15=stage1.5/stage15.bin
STAGE1=stage1.bin

# source files
 	# not really source, its
				# meat for the linker
STAGE1_SRC=stage1.asm

.PHONY: all
all: $(STAGE15) $(STAGE1)
	$(DD) if=$(STAGE15) of=$(STAGE1) seek=1 ibs=512
	

$(STAGE15):
	$(MAKE) FLAGS="$(FLAGS)" -C stage1.5

$(STAGE1): $(STAGE1_SRC)
	$(AS) $(BINASFLAGS) $(STAGE1_SRC) -o $(STAGE1)

.PHONY: clean
clean:
	$(MAKE) -C stage1.5 clean
	#rm -v *.bin

