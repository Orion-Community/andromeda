CC=gcc
LD=ld
CFLAGS=-I../include/ -nostdlib -fno-builtin -nostdinc -fno-stack-protector -m32 -c -D X86 -D __INTEL -mtune=i686 -march=i386
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

TARGETS=alloc.c heap.c map.c memory.c paging.c test.c

.PHONY: all
all: mm.o

mm.o: $(TARGETS)
	$(CC) $(TARGETS) $(CFLAGS)
	$(LD) -o mm.o *.o $(LDFLAGS)
	
.PHONY: clean
clean:
	rm -v *.o