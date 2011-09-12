include ../../scripts/makeIncl
MAKEEND=FLAGS="$(FLAGS) " DEFS="$(DEFS)" $(DEFS)
CFLAGS+=-I../../include/

LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

INT=interrupts.o
MM=mm.o

.PHONY: all
all: arch.o


arch.o: $(INTERRUPTS) $(MM)
	$(CC) $(CFLAGS) $(FLAGS) */*.c
	$(MAKE) -C asm $(MAKEEND)
	$(LD) -o arch.o *.o $(LDFLAGS)

$(MM):
	$(MAKE) -C interrupts/

.PHONY: clean
clean:
	$(MAKE) -C asm clean
	rm -fv *.o
