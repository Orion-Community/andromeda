include ../scripts/makeIncl
CFLAGS+=-I../include
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

INCLUDES=../include/fs/stream.h ../include/fs/file.h ../include/fs/vfs.h\
../include/fs/path.h

.PHONY: all
all: stream.o file.o vfs.o path.o test.o
	$(LD) -o fs.o *.o $(LDFLAGS)
	mv fs.o ../

stream.o: stream.c
	$(CC) $(FLAGS) $(CFLAGS) stream.c

file.o: file.c
	$(CC) $(FLAGS) $(CFLAGS) file.c

vfs.o: vfs.c
	$(CC) $(FLAGS) $(CFLAGS) vfs.c

path.o: path.c
	$(CC) $(FLAGS) $(CFLAGS) path.c

test.o: test.c
	$(CC) $(FLAGS) $(CFLAGS) test.c