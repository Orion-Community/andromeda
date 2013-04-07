include ../../../scripts/makeIncl
CFLAGS+=-I../../../../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

INCLUDES:=
TARGETS:=page_alloc_init.o page_allocate.o

.PHONY: all
all: page_alloc.o

.PHONY: page_alloc.o
page_alloc.o: $(TARGETS)
	$(LD) $(LDFLAGS) -o page_alloc.o *.o
	cp -vf page_alloc.o ../page_alloc2.o
