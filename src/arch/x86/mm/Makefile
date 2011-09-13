include ../../../scripts/makeIncl
MAKEEND=FLAGS="$(FLAGS) " DEFS="$(DEFS)" $(DEFS)
CFLAGS+=-I../../../include/

LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

## targets
MM=mm.o

## deps
MM_=gdt.o

## sources
GDT_SRC=gdt.c

.PHONY: all
all: $(MM)

$(MM): $(MM_)
	$(LD) $(LDFLAGS) $(MM_) -o $(MM)

gdt.o: $(GDT_SRC)
	$(CC) $(CFLAGS) $(GDT_SRC) -o gdt.o

.PHONY: clean
clean:
	rm -fv *.o
