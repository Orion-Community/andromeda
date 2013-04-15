include ../../scripts/makeIncl
CFLAGS+=-I../../../include/

LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

TARGETS=out.o vga_text.o
DEPS=include/out.h

.PHONY: all
all: $(TARGETS)
	$(LD) $(LDFLAGS) -o ../text.o *.o

%o: %c
	$(CC) $(CFLAGS) $(FLAGS) $< -o $@

.PHONY: clean
clean:
	rm -vf *.o
