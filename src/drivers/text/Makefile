include ../../scripts/makeIncl
CFLAGS+=-I../../include/

LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

TARGETS=out.c
DEPS=include/out.h

.PHONY: all
all: text.o vga_text.o
	$(LD) $(LDFLAGS) -o txt.o *o
	mv -vf txt.o ../text.o

text.o: $(TARGETS) $(DEPS)
	$(CC) $(CFLAGS) $(FLAGS) $(TARGETS) -o text.o

vga_text.o: vga_text.c
	$(CC) $(CFLAGS) $(FLAGS) vga_text.c -o vga_text.o
