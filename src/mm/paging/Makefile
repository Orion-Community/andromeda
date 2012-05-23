include ../../scripts/makeIncl
CFLAGS+=-I../../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386


.PHONY: all
all: paging.o
	mv -fv paging.o ../

paging.o: boot_alloc.o page_alloc.o pte.o
	$(LD) -o paging.o *.o $(LDFLAGS)

boot_alloc.o: boot_alloc.c
	$(CC) $(FLAGS) $(CFLAGS) boot_alloc.c

page_alloc.o: page_alloc.c
	$(CC) $(FLAGS) $(CFLAGS) page_alloc.c

pte.o:
	$(MAKE) -C pte/

.PHONY: clean
clean:
	rm -fv *.o
