CC=gcc
LD=ld
CFLAGS=-I../include/ -nostdlib -fno-builtin -nostdinc -fno-stack-protector -m32 -c -D X86 -D __INTEL -mtune=i686 -march=i386
LDFLAGS=--allow-shlib-undefined -shared -nostdlib --oformat=elf32-i386 -melf_i386

.PHONY: all
all: panic.o

panic.o: panic.c
	$(CC) $(CFLAGS) panic.c