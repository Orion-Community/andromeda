include ../../../scripts/makeIncl
MAKEEND=FLAGS="$(FLAGS) " DEFS="$(DEFS)" $(DEFS)
CFLAGS+=-I../../../include/
AS=nasm
ASFLAGS=-felf32 -I"../../../include/" $(FLAGS)

LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

## targets
MM=mm.o
IRQ30=irq30.o

## deps
MM_=gdt.o irq30.o paging.o page_alloc.o

## sources
GDT_SRC=gdt.c

IRQ30_SRC=cIRQ30.asm
MM_DEPS=irq30.o


.PHONY: all
all: $(MM)

$(MM): $(MM_)
	$(LD) $(LDFLAGS) $(MM_) -o $(MM)

page_alloc.o:
	$(MAKE) -C page_alloc/

gdt.o: $(GDT_SRC)
	$(CC) $(CFLAGS) $(GDT_SRC) -o gdt.o

$(IRQ30): $(IRQ30_SRC)
	$(AS) $(ASFLAGS) -o $(IRQ30) $(IRQ30_SRC)

paging.o: paging.c
	$(CC) $(CFLAGS) paging.c -o paging.o

.PHONY: clean
clean:
	rm -fv *.o
