# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../../include/" -I"../../interface/" $(FLAGS)
MAKE=make

# targets
STAGE15=stage15.o
#A20=a20.o

# source
STAGE15_SRC=stage15.asm

#A20_SRC=a20.asm

# headers
#A20_HDR=../../include/a20.asmh

.PHONY: all
all: $(STAGE15) $(MMAP)

$(STAGE15): $(STAGE15_SRC)
	$(AS) $(ASFLAGS) $(STAGE15_SRC) -o $(STAGE15)

#$(A20): $(A20_SRC) $(A20_HDR)
#	$(AS) $(ASFLAGS) $(A20_SRC) -o $(A20)

.PHONY: clean
clean:
	rm -v *.o
