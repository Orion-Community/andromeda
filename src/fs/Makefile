include ../scripts/makeIncl
CFLAGS=-I../include

INCLUDES=../include/fs/file.h

all: file.o
	$(LD) -o fs *.o $(LDFLAGS)

file.o: file.c
	$(CC) $(CFLAGS) $(FLAGS) file.c