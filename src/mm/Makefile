include ../scripts/makeIncl
CFLAGS+=-I../../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

INCLUDES=../../include/mm/heap.h ../../include/mm/map.h ../../include/mm/memory.h
INCLUDES+=../../include/mm/paging.h

TARGETS=map.o memory.o test.o slob.o slab.o paging.o

.PHONY: all paging.o slab.o slob.o
all: mm.o
	mv -fv mm.o ../

mm.o: $(TARGETS)
	$(LD) $(LDFLAGS) -o mm.o *.o

slob.o:
	$(MAKE) -C slob/

slab.o:
	$(MAKE) -C slab/

paging.o:
	$(MAKE) -C paging/

.PHONY: clean
clean:
	rm -fv *.o
