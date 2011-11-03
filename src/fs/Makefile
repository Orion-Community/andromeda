include ../scripts/makeIncl
CFLAGS+=-I../include
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

INCLUDES=../include/fs/file.h

.PHONY: all
all: file.o
	$(LD) -o fs.o *.o $(LDFLAGS)
	mv fs.o ../

file.o: file.c
	$(CC) $(FLAGS) $(CFLAGS) file.c