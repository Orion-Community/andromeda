include ../../scripts/makeIncl
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386
CFLAGS+=-I../../../include/

DEPS=VGA.c graphics.c text.c
TARGETS=vga.o
HEADERS=Include/VGA.h Include/graphics.h Include/text.h

.PHONY: all
all: $(TARGETS)
	mv -fv vga.o ../graphics.o;

$(TARGETS): $(DEPS) $(HEADERS)
	$(CC) $(CFLAGS) $(FLAGS) $(DEPS)
	$(LD) $(LDFLAGS) -o $(TARGETS) *.o

.PHONY: clean
clean:
	rm -fv *.o
