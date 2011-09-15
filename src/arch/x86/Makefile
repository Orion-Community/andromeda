include ../../scripts/makeIncl
MAKEEND=FLAGS="$(FLAGS) " DEFS="$(DEFS)" $(DEFS)
CFLAGS+=-I../../include/

LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

## TARGETS
ARCH=arch.o

## DEPS
DEPS=kernel.o mm.o asm.o

.PHONY: all
all: arch.o


arch.o: $(DEPS)
	$(LD) -o arch.o $(DEPS) $(LDFLAGS)

mm.o:
	$(MAKE) $(MAKEEND) -C mm/
	mv -fv mm/mm.o ./

kernel.o:
	$(MAKE) $(MAKEEND) -C kernel/
	mv kernel/kernel.o ./

asm.o:
	$(MAKE) -C asm $(MAKEEND)
	mv -fv asm/asm.o ./

.PHONY: clean
clean:
	$(MAKE) -C kernel clean
	$(MAKE) -C asm clean
	$(MAKE) -C mm clean
	rm -fv *.o
