include ../../scripts/makeIncl
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386
CFLAGS+=-I../../include/

DEPS=bus.c
TARGETS=virtual.o

.PHONY: all
all: $(TARGETS)
	mv virtual.o ../

$(TARGETS): $(DEPS)
	$(CC) $(CFLAGS) $(FLAGS) $(DEPS)
	$(LD) $(LDFLAGS) -o $(TARGETS) *.o

.PHONY: clean
clean:
	rm -fv *.o
