include ../../scripts/makeIncl
CFLAGS+=-I../../../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386


.PHONY: all
all: slab.o
	 mv -fv slab.o ../

slab.o: slab_init.o slab_alloc.o
	$(LD) -o slab.o *.o $(LDFLAGS)

.PHONY: clean
clean:
	rm -fv *.o
