CC=gcc
LD=ld
CFLAGS=-I../../include/ -nostdlib -fno-builtin -nostdinc -fno-stack-protector -m32 -c -D X86 -D __INTEL -mtune=i686 -march=i386
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

TARGETS=input.c output.c tty.c

.PHONY: all
all: tty.o

tty.o: $(TARGETS)
	$(CC) $(TARGETS) $(FLAGS) $(CFLAGS)
	$(LD) -o tty.o *.o $(LDFLAGS)
	
.PHONY: clean
clean:
	rm -v *.o