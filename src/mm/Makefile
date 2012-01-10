include ../scripts/makeIncl
CFLAGS+=-I../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

INCLUDES=../include/mm/heap.h ../include/mm/map.h ../include/mm/memory.h\
                                                          ../include/mm/paging.h


.PHONY: all
all: mm.o
	mv -fv mm.o ../

mm.o: alloc.o heap.o map.o memory.o test.o
	$(LD) -o mm.o *.o $(LDFLAGS)

alloc.o: alloc.c $(INCLUDES)
	$(CC) $(FLAGS) $(CFLAGS) alloc.c

heap.o: heap.c $(INCLUDES)
	$(CC) $(FLAGS) $(CFLAGS) heap.c

map.o: map.c $(INCLUDES)
	$(CC) $(FLAGS) $(CFLAGS) map.c

memory.o: memory.c $(INCLUDES)
	$(CC) $(FLAGS) $(CFLAGS) memory.c

test.o: test.c $(INCLUDES)
	$(CC) $(FLAGS) $(CFLAGS) test.c


.PHONY: clean
clean:
	rm -fv *.o
