include ../../scripts/makeIncl
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386
CFLAGS+= -I../../include

TARGETS=fs.c

.PHONY: all
all: vfs.o
	mv -fv vfs.o ../

vfs.o: $(TARGETS)
	$(CC) $(TARGETS) $(FLAGS) $(CFLAGS)
	$(LD) -o vfs.o *.o $(LDFLAGS)
	
.PHONY: clean
clean:
	rm -fv *.o
