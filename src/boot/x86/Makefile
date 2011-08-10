# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"include/" -I"interface/" $(FLAGS)
BINASFLAGS=-fbin -I"include/" -I"interface/" $(FLAGS)
MAKE=make
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386 -e pmodemain
LD=ld
DD=dd

# targets
BOOT=boot.o
MASTERBOOT=masterboot.bin
STAGE15=stage1/stage15.bin
STAGE2=stage2/stage2.bin
STAGE1=stage1.bin
# sources
MASTERBOOT_SRC=masterboot.asm

# headers
MASTERBOOT_HDR=include/masterboot.asmh

.PHONY: all
all: $(BOOT) $(MASTERBOOT)
	mv -v stage1/$(STAGE1) ./
	$(DD) if=$(STAGE2) of=$(STAGE1) ibs=512 seek=3

$(MASTERBOOT): $(MASTERBOOT_SRC) $(MASTERBOOT_HDR)
	$(AS) $(BINASFLAGS) $(MASTERBOOT_SRC) -o $(MASTERBOOT)

$(BOOT):
	$(MAKE) FLAGS="$(FLAGS)" -C stage1
	$(MAKE) FLAGS="$(FLAGS)" -C stage2
	$(LD) $(LDFLAGS) stage2/pmodemain.o -o $(BOOT)

.PHONY: clean
clean:
	$(MAKE) -C stage1 clean
	$(MAKE) -C stage2 clean
	
