include ../../scripts/makeIncl
CFLAGS+=-I../../../include
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386


.PHONY: all
all: pfs.o
	$(LD) -o procfs.o *.o $(LDFLAGS)
	mv procfs.o ../

pfs.o: procfs.c
	$(CC) $(CFLAGS) $(FLAGS) procfs.c -o pfs.o
