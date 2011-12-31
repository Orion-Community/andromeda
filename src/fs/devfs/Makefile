include ../../scripts/makeIncl
CFLAGS+=-I../../include
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386


.PHONY: all
all: devfs.o
	$(LD) -o dfs.o *.o $(LDFLAGS)
	mv dfs.o ../

devfs.o: devfs.c
	$(CC) $(CFLAGS) $(FLAGS) devfs.c
