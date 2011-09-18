include ../scripts/makeIncl
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386
MAKE=make
CFLAGS+= -I../include
# TARGETS=io.c

.PHONY: all
all: fs.o
	mv -v fs.o ../

io.c:
	$(CC) io.c $(FLAGS) $(CFLAGS)

vfs.o:
	$(MAKE) -C vfs

fs.o: io.o vfs.o
	$(LD) -o fs.o *.o $(LDFLAGS)
	
.PHONY: clean
clean:
	rm -fv *.o
	$(MAKE) -C vfs/ clean
