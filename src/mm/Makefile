# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"include/" $(FLAGS)
CC=gcc
CCFLAGS=-I"../include" -c -m32 -nostdlib -nodefaultlibs -fno-stack-protector -fno-builtin -nostdinc $(FLAGS)
MAKE=make
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

# targets
MM=mm.o
MM_DEPS=mmap.o mmap2.o

# source files
MMAP_SRC=mmap.asm
MMAP2_SRC=mmap.c

#headers
MMAP_HDR=include/mmap.h

.PHONY: all
all: $(MM)

$(MM): $(MM_DEPS)
	$(LD) $(LDFLAGS) $(MM_DEPS) -o $(MM)

mmap.o:
	$(AS) $(ASFLAGS) $(MMAP_SRC) -o mmap.o

mmap2.o: $(MMAP_HDR)
	$(CC) $(CCFLAGS) $(MMAP2_SRC) -o mmap2.o

.PHONY: clean
clean:
	rm -v *.o
