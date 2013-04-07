include ../../scripts/makeIncl
CFLAGS+=-I../../../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386


.PHONY: all
all: paging.o
	mv -fv paging.o ../

paging.o: page_alloc.o vm.o mboot.o x86.o page_alloc2.o
	$(LD) -o paging.o *.o $(LDFLAGS)

page_alloc2.o:
	$(MAKE) -C page_alloc/

vm.o:
	$(MAKE) -C vm/

.PHONY: clean
clean:
	rm -fv *.o
