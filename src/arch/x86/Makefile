include ../../scripts/makeIncl
MAKEEND=FLAGS="$(FLAGS) " DEFS="$(DEFS)" $(DEFS)
CFLAGS+=-I../../include/

LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

## TARGETS
X86=x86.o

## DEPS
DEPS=kernel.o mm.o asm.o boot.o kern.o

.PHONY: all
all: x86.o


x86.o: $(DEPS)
	$(LD) -o $(X86) $(DEPS) $(LDFLAGS)

mm.o:
	$(MAKE) $(MAKEEND) -C mm/
	mv -fv mm/mm.o ./

kernel.o:
	$(MAKE) $(MAKEEND) -C kernel/
	mv kernel/kernel.o ./

boot.o:
	$(MAKE) $(MAKEEND) -C boot/
	mv boot/boot.o ./

asm.o:
	$(MAKE) -C asm $(MAKEEND)
	mv -fv asm/asm.o ./

kern.o:
	$(MAKE) $(MAKEEND) -C kern
	mv -vf kern/kern.o ./