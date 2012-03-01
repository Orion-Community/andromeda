include ../scripts/makeIncl
CFLAGS+=-I../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

INCLUDES=../include/mm/heap.h ../include/mm/map.h ../include/mm/memory.h\
                                                          ../include/mm/paging.h


.PHONY: all paging.o slab.o slob.o
all: mm.o
	mv -fv mm.o ../

mm.o: map.o memory.o test.o slob.o slab.o paging.o
	$(LD) -o mm.o *.o $(LDFLAGS)

slob.o:
	$(MAKE) -C slob/

slab.o:
	$(MAKE) -C slab/

map.o: map.c $(INCLUDES)
	$(CC) $(FLAGS) $(CFLAGS) map.c

memory.o: memory.c $(INCLUDES)
	$(CC) $(FLAGS) $(CFLAGS) memory.c

test.o: test.c $(INCLUDES)
	$(CC) $(FLAGS) $(CFLAGS) test.c

paging.o:
	$(MAKE) -C paging/


.PHONY: clean
clean:
	rm -fv *.o
