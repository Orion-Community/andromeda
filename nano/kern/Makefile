CC=gcc
LD=ld
CFLAGS=-I../include/ -nostdlib -fno-builtin -nostdinc -fno-stack-protector -m32 -c -D X86 -D __INTEL -mtune=i686 -march=i386
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

ELF=elf.c exec.c
TIMER=timer.c
CORE=kmain.c

.PHONY: all
all: kern.o

kern.o: load.o timer.o core.o
	$(LD) $(LDFLAGS) *.o -o kern.o
load.o: $(ELF)
	$(CC) $(CFLAGS) $(FLAGS) $(ELF)

timer.o: $(TIMER)
	$(CC) $(CFLAGS) $(FLAGS) $(TIMER)

core.o: $(CORE)
	$(CC) $(CFLAGS) $(FLAGS) $(CORE)

.PHONY: clean
clean:
	rm -v *.o