CC=gcc
LD=ld
CFLAGS=-I../include/ -nostdlib -fno-builtin -nostdinc -fno-stack-protector -m32 -c -D X86 -D __INTEL -mtune=i686 -march=i386
LDFLAGS=--allow-shlib-undefined -shared -nostdlib --oformat=elf32-i386 -melf_i386

ELF=elf.c exec.c
TIMER=timer.c
CORE=kmain.c

.PHONY: all
all: load.o timer.o core.o

load.o: $(ELF)
	$(CC) $(CFLAGS) $(ELF)
	$(LD) $(LDFLAGS) elf.o exec.o -o load.o

timer.o: $(TIMER)
	$(CC) $(CFLAGS) $(TIMER)

core.o: $(CORE)
	$(CC) $(CFLAGS) $(CORE)

.PHONY: clean
clean:
	rm -v *.o