include ../../scripts/makeIncl
CFLAGS+=-I../../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386


.PHONY: all
all: paging.o
	mv -fv paging.o ../

paging.o: page_alloc.o pte.o mboot.o x86.o vmem.o vmem_buddy.o
	$(LD) -o paging.o *.o $(LDFLAGS)

page_alloc.o: page_alloc.c
	$(CC) $(FLAGS) $(CFLAGS) page_alloc.c

page_alloc2.o:
	$(MAKE) -C page_alloc/

pte.o:
	$(MAKE) -C pte/

x86.o: x86.c
	$(CC) $(FLAGS) $(CFLAGS) x86.c

mboot.o: mboot.c
	$(CC) $(FLAGS) $(CFLAGS) mboot.c

vmem.o: vmem.c
	$(CC) $(FLAGS) $(CFLAGS) vmem.c

vmem_buddy.o: vmem_buddy.c
	$(CC) $(FLAGS) $(CFLAGS) vmem_buddy.c

.PHONY: clean
clean:
	rm -fv *.o
