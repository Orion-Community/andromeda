include ../../scripts/makeIncl
CFLAGS+=-I../../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386


.PHONY: all
all: slab_alloc.o
	mv -fv slab_alloc.o ../slab.o

slab_alloc.o:
	$(LD) -o slab_alloc.o *.o $(LDFLAGS)

slab.o:
	$(CC) $(FLAGS) $(CFLAGS) slab.c

.PHONY: clean
clean:
	rm -fv *.o
