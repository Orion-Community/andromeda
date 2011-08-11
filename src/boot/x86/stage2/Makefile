# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../include/" -I"../interface/" $(FLAGS)
BINASFLAGS=-fbin -I"../include/" -I"../interface/" $(FLAGS)
MAKE=make

# targets
STAGE2=stage2.bin
PMODEMAIN=pmodemain.o
#MMAP=mmap.o

# source
STAGE2_SRC=stage2.asm
PMODEMAIN_SRC=pmodemain.asm
#MMAP_SRC=mmap.asm

# headers
MMAP_HDR=../include/mmap.asmh
STAGE15_HDR=../include/stage15.asmh

.PHONY: all
all: $(STAGE2) $(PMODEMAIN)

$(STAGE2): $(STAGE2_SRC) $(MMAP_HDR) $(STAGE15_HDR)
	$(AS) $(BINASFLAGS) $(STAGE2_SRC) -o $(STAGE2) -D __STAGE2

$(PMODEMAIN): $(PMODEMAIN_SRC)
	$(AS) $(ASFLAGS) $(PMODEMAIN_SRC) -o $(PMODEMAIN)

#$(MMAP): $(MMAP_HDR) $(MMAP_HDR)
	#$(AS) $(ASFLAGS) $(MMAP_SRC) -o $(MMAP)

.PHONY: clean
clean:
	rm -v *.o
	rm -v *.bin
