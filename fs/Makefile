CC=gcc
LD=ld
CFLAGS=-I../include/ -nostdlib -fno-builtin -nostdinc -fno-stack-protector -m32 -c -D X86 -D __INTEL -mtune=i686 -march=i386
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386
MAKE=make

# TARGETS=io.c

.PHONY: all
all: fs.o
	mv -v vfs.o ../

io.c:
	$(CC) io.c $(FLAGS) $(CFLAGS)

vfs.o:
	$(MAKE) -C vfs

fs.o: io.c vfs.o
	$(LD) -o fs.o *.o $(LDFLAGS)
	
.PHONY: clean
clean:
	rm -fv *.o
	$(MAKE) -C vfs/ clean
