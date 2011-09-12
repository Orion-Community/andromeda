include ../scripts/makeIncl
CFLAGS+=-I../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

TARGETS=alloc.c heap.c map.c memory.c paging.c test.c

.PHONY: all
all: mm.o
	mv -fv mm.o ../

mm.o: $(TARGETS)
	$(CC) $(TARGETS) $(FLAGS) $(CFLAGS)
	$(LD) -o mm.o *.o $(LDFLAGS)
	
.PHONY: clean
clean:
	rm -fv *.o
