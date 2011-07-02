CC=gcc
CFLAGS=-I../../include/ -nostdlib -fno-builtin -nostdinc -fno-stack-protector -m32 -c -D X86 -D __INTEL -mtune=i686 -march=i386

TARGETS=out.c
DEPS=include/out.h

.PHONY: all
all: text.o

text.o: $(TARGETS) $(DEPS)
	$(CC) $(CFLAGS) $(FLAGS) $(TARGETS) -o text.o