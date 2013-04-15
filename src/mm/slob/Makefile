include ../../scripts/makeIncl
CFLAGS+=-I../../../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386


.PHONY: all
all: slob.o
	mv -fv slob.o ../

slob.o: alloc.o heap.o
	$(LD) -o slob.o *.o $(LDFLAGS)

.PHONY: clean
clean:
	rm -fv *.o
