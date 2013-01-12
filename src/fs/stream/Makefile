include ../../scripts/makeIncl

CFLAGS+=-I../../include
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

TARGETS=stream.o

.PHONY: all
all: $(TARGETS)
	ld -o str.o $(TARGETS) $(LDFLAGS)
	mv str.o ../stream.o

%o: %c
	$(CC) $(CFLAGS) $(FLAGS) $< -o $@
