include ../scripts/makeIncl
CFLAGS+=-I../include
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

INCLUDES=../include/fs/stream.h ../include/fs/file.h ../include/fs/vfs.h\
../include/fs/path.h

TARGETS=path.o vfs.o

.PHONY: all procfs.o ramfs.o
all: path.o ramfs.o vfs.o procfs.o
	$(LD) -o fs.o *.o $(LDFLAGS)
	mv fs.o ../

%o: %c
	$(CC) $(CFLAGS) $(FLAGS) $< -o $@

ramfs.o:
	$(MAKE) -C ramfs/
	mv rfs.o ramfs.o

procfs.o:
	$(MAKE) -C procfs/

.PHONY: clean
clean:
	rm -vf *.o
