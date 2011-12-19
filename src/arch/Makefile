MAKEEND=FLAGS="$(FLAGS) " DEFS="$(DEFS)" $(DEFS)
MAKE=make


LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

.PHONY: all
all: arch.o
	mv -fv arch.o ../

arch.o: x86.o
	$(LD) $(LDFLAGS) *.o -o arch.o

x86.o:
	
	$(MAKE) $(MAKEEND) -C x86
	mv -fv x86/x86.o ./
