CC=gcc
LD=ld
CFLAGS=-I../include/ -nostdlib -fno-builtin -nostdinc -fno-stack-protector -m32 -c -D X86 -D __INTEL -mtune=i686 -march=i386
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

DEPS=text.c TTY.c
TARGETS=vga.o
HEADERS=include/graphics.h include/TTY.h include/vga.h

.PHONY: all
all: $(TARGETS)

$(TARGETS): $(DEPS) $(HEADERS)
	$(CC) $(CFLAGS) $(FLAGS) $(DEPS)
	$(LD) $(LDFLAGS) -o $(TARGETS) *.o

.PHONY: clean
clean:
	rm -v *.o
