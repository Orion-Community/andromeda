include ../scripts/makeIncl
CFLAGS+=-I../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

# glbl target
ERROR=error.o

#deps
DEPS=panic.o

.PHONY: all
all: $(DEPS)
	$(LD) $(LDFLAGS) -o $(ERROR) $(DEPS)
	mv -fv $(ERROR) ../

panic.o: panic.c
	$(CC) $(CFLAGS) $(FLAGS) panic.c

.PHONY: clean
clean:
	rm -fv *.o
