include ../../scripts/makeIncl

CFLAGS+=-I../../../include
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

TARGETS=pipe.o

.PHONY: all
all: $(TARGETS)
	ld -o p.o $(TARGETS) $(LDFLAGS)
	mv p.o ../pipe.o

%o: %c
	$(CC) $(CFLAGS) $(FLAGS) $< -o $@
