include ../scripts/makeIncl
CFLAGS+=-I../include
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

INCLUDES=../include/fs/stream.h ../include/fs/file.h ../include/fs/vfs.h

.PHONY: all
all: stream.o file.o
	$(LD) -o fs.o *.o $(LDFLAGS)
	mv fs.o ../

stream.o: stream.c
	$(CC) $(FLAGS) $(CFLAGS) stream.c

file.o: file.c
	$(CC) $(FLAGS) $(CFLAGS) file.c

vfs.o: vfs.c
	$(CC) $(FLAGS) $(CFLAGS) vfs.c