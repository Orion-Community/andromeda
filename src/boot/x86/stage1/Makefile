# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../include/" -I"../interface/" $(FLAGS)
BINASFLAGS=-fbin -I"../include/" -I"../interface/" $(FLAGS)
MAKE=make
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386
LD=ld

# targets

STAGE15=stage1_5.o
STAGE1=stage1.bin

# source files
STAGE15_SRC=stage1.5/stage15.o 	# not really source, its
				# meat for the linker
STAGE1_SRC=stage1.asm

.PHONY: all
all: $(STAGE15_SRC) $(STAGE1)
	$(LD) $(LDFLAGS) $(STAGE15_SRC) -o $(STAGE15)

$(STAGE15_SRC):
	$(MAKE) FLAGS="$(FLAGS)" -C stage1.5

$(STAGE1): $(STAGE1_SRC)
	$(AS) $(BINASFLAGS) $(STAGE1_SRC) -o $(STAGE1)

.PHONY: clean
clean:
	$(MAKE) -C stage1.5 clean
	rm -v *.o

