include ../../../scripts/makeIncl
MAKEEND=FLAGS="$(FLAGS) " DEFS="$(DEFS)" $(DEFS)
CFLAGS+=-I../../../../include/
AS=nasm
ASFLAGS=-felf32 -I"../../../../include/" $(FLAGS)

LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

## targets
MM=mm.o
IRQ30=irq30.o

## deps
MM_=gdt.o irq30.o page_table.o page_api.o
IRQ30_SRC=cIRQ30.asm
MM_DEPS=irq30.o


.PHONY: all
all: $(MM)

$(MM): $(MM_)
	$(LD) $(LDFLAGS) $(MM_) -o $(MM)


$(IRQ30): $(IRQ30_SRC)
	$(AS) $(ASFLAGS) -o $(IRQ30) $(IRQ30_SRC)


.PHONY: clean
clean:
	rm -fv *.o
