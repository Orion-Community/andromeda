CC=gcc
LD=ld
CFLAGS=-I../include/ -nostdlib -fno-builtin -nostdinc -fno-stack-protector -m32 -c -D X86 -D __INTEL -mtune=i686 -march=i386
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

TARGETS=heap.c

.PHONY: all
all: coremm.o
	mv -v coremm.o ../

coremm.o: $(TARGETS)
	$(CC) $(TARGETS) $(FLAGS) $(CFLAGS)
	$(LD) -o coremm.o *.o $(LDFLAGS)
	
.PHONY: clean
clean:
	rm -fv *.o
