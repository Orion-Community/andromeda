MAKEEND=FLAGS="$(FLAGS) " DEFS="$(DEFS)" $(DEFS)
MAKE=make


LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

.PHONY: all
all: x86
	mv -fv arch.o ../

.PHONY: x86
x86:

	$(MAKE) $(MAKEEND) -C x86
	mv -fv x86/x86.o ./arch.o


.PHONY: avr
avr:
	$(MAKE) $(MAKEEND) -C avr
	mv -fv avr/avr.o ./arch.o
