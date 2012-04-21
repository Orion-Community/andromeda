include ../../scripts/makeIncl
CFLAGS+=-I../../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386


.PHONY: all
all: slab.o
	 mv -fv slab.o ../

slab.o: slab_init.o
	$(LD) -o slab.o *.o $(LDFLAGS)

slab_init.o: slab_init.c
	$(CC) $(FLAGS) $(CFLAGS) slab_init.c

slab_alloc.o: slab_alloc.c
	$(CC) $(FLAGS) $(CFLAGS) slab_alloc.c

.PHONY: clean
clean:
	rm -fv *.o
