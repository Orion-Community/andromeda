CC=gcc
CFLAGS=-I../../include/ -Iinclude/ -nostdlib -fno-builtin -nostdinc -fno-stack-protector -m32 -c -D X86 -D __INTEL -mtune=i686 -march=i386
ASM=nasm
MAKE=make

.PHONY: all
all:
	$(CC) $(CFLAGS) */*.c
	$(MAKE) -C asm

.PHONY: clean
clean:
	$(MAKE) -C asm clean
	rm -v *.o
