include ../scripts/makeIncl
CFLAGS+=-I../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

.PHONY: all
all: panic.o
	mv -fv panic.o ../

panic.o: panic.c
	$(CC) $(CFLAGS) $(FLAGS) panic.c

.PHONY: clean
clean:
	rm -fv *.o
