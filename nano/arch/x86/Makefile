CC=gcc
CFLAGS=-I../../include/ -Iinclude/ -nostdlib -fno-builtin -nostdinc -fno-stack-protector -m32 -c -D X86 -D __INTEL -mtune=i686 -march=i386
ASM=nasm
MAKE=make

LD=ld
LDFLAGS=--allow-shlib-undefined -shared -nostdlib --oformat=elf32-i386 -melf_i386

.PHONY: all
all: arch.o

arch.o:
	$(CC) $(CFLAGS) */*.c
	$(MAKE) -C asm
	$(LD) -o arch.o *.o  $(LDFLAGS)

.PHONY: clean
clean:
	$(MAKE) -C asm clean
	rm -v *.o
