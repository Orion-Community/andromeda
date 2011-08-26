# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../include/" $(FLAGS)
CC=gcc
CCFLAGS=-I"../include" -c -m32 -nostdlib -nodefaultlibs -fno-stack-protector -fno-builtin -nostdinc $(FLAGS)
MAKE=make
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

# targets
MM=mm.o
MM_DEPS=mmap.o mmap2.o heap.o memory.o

# source files
MMAP=mmap.asm
MMAP2=mmap.c
ALLOC=alloc.c
HEAP=heap.c
MEMORY=memory.c

.PHONY: all
all: $(MM)

$(MM): $(MM_DEPS)
	$(LD) $(LDFLAGS) $(MM_DEPS) -o $(MM)

mmap.o:
	$(AS) $(ASFLAGS) $(MMAP) -o mmap.o

mmap2.o:
	$(CC) $(CCFLAGS) $(MMAP2) -o mmap2.o
	
alloc.o: $(ALLOC)
	$(CC) $(CCFLAGS) $(ALLOC) -o alloc.o
	
heap.o: $(HEAP)
	$(CC) $(CCFLAGS) $(HEAP) -o heap.o
	
memory.o: $(MEMORY)
	$(CC) $(CCFLAGS) $(MEMORY) -o memory.o

.PHONY: clean
clean:
	rm -v *.o
