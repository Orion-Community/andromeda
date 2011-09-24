include ../scripts/makeIncl
CFLAGS+=-I../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

INCLUDES=../include/mm/heap.h ../include/mm/map.h ../include/mm/memory.h\
                                                          ../include/mm/paging.h


.PHONY: all
all: mm.o
	mv -fv mm.o ../

mm.o: alloc.o heap.o map3.o memory.o paging.o test.o
	$(LD) -o mm.o *.o $(LDFLAGS)

alloc.o: alloc.c $(INCLUDES)
	$(CC) $(FLAGS) $(CFLAGS) alloc.c

heap.o: heap.c $(INCLUDES)
	$(CC) $(FLAGS) $(CFLAGS) heap.c

map.o: map.c $(INCLUDES)
	$(CC) $(FLAGS) $(CFLAGS) map.c

map2.o: map2.c $(INCLUDES)
	$(CC) $(FLAGS) $(CFLAGS) map2.c

map3.o: map3.c $(INCLUDES)
	$(CC) $(FLAGS) $(CFLAGS) map3.c

memory.o: memory.c $(INCLUDES)
	$(CC) $(FLAGS) $(CFLAGS) memory.c

paging.o: paging.c $(INCLUDES)
	$(CC) $(FLAGS) $(CFLAGS) paging.c

test.o: test.c $(INCLUDES)
	$(CC) $(FLAGS) $(CFLAGS) test.c

	
.PHONY: clean
clean:
	rm -fv *.o
