include ../scripts/makeIncl
CFLAGS+=-I../include
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

INCLUDES=../include/fs/stream.h

.PHONY: all
all: stream.o
	$(LD) -o fs.o *.o $(LDFLAGS)
	mv fs.o ../

stream.o: stream.c
	$(CC) $(FLAGS) $(CFLAGS) stream.c