include ../../scripts/makeIncl
CFLAGS+=-I../../../include
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386


.PHONY: all
all: ramfs.o
	$(LD) -o rfs.o *.o $(LDFLAGS)
	mv rfs.o ../

ramfs.o: ramfs.c
	$(CC) $(CFLAGS) $(FLAGS) ramfs.c
