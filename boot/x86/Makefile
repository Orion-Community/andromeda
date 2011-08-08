# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"include/" -I"interface/" $(FLAGS)
BINASFLAGS=-fbin -I"include/" -I"interface/" $(FLAGS)
MAKE=make
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386 -e main
LD=ld

# targets
BOOT=boot.o
MASTERBOOT=masterboot.bin
# sources
STAGE15_SRC=stage1/stage1_5.o # not source, linker meat
STAGE2_SRC=stage2/stage2.o stage2/mmap.o stage2/pmodemain.o
MASTERBOOT_SRC=masterboot.asm

# headers
MASTERBOOT_HDR=include/masterboot.asmh

.PHONY: all
all: $(BOOT) $(MASTERBOOT)
	mv -v stage1/stage1.bin ./

$(MASTERBOOT): $(MASTERBOOT_SRC) $(MASTERBOOT_HDR)
	$(AS) $(BINASFLAGS) $(MASTERBOOT_SRC) -o $(MASTERBOOT)

$(BOOT):
	$(MAKE) FLAGS="$(FLAGS)" -C stage1 # make stage 1 + stage 1.5
	$(MAKE) FLAGS="$(FLAGS)" -C stage2
	$(LD) $(LDFLAGS) $(STAGE15_SRC) $(STAGE2_SRC) -o $(BOOT)

.PHONY: clean
clean:
	$(MAKE) -C stage1 clean
	$(MAKE) -C stage2 clean
	
