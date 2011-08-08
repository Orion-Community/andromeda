# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../include/" -I"../interface/" $(FLAGS)
MAKE=make

# targets
STAGE2=stage2.o
PMODEMAIN=pmodemain.o
MMAP=mmap.o

# source
STAGE2_SRC=stage2.asm
PMODEMAIN_SRC=pmodemain.asm
MMAP_SRC=mmap.asm

# headers
MMAP_HDR=../include/mmap.asmh

.PHONY: all
all: $(STAGE2) $(PMODEMAIN) $(MMAP)

$(STAGE2): $(STAGE2_SRC)
	$(AS) $(ASFLAGS) $(STAGE2_SRC) -o $(STAGE2)

$(PMODEMAIN): $(PMODEMAIN_SRC)
	$(AS) $(ASFLAGS) $(PMODEMAIN_SRC) -o $(PMODEMAIN)

$(MMAP): $(MMAP_HDR) $(MMAP_HDR)
	$(AS) $(ASFLAGS) $(MMAP_SRC) -o $(MMAP)

.PHONY: clean
clean:
	rm -v *.o
