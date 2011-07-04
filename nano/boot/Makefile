CC=gcc
LD=ld
CFLAGS=-I../../include/ -nostdlib -fno-builtin -nostdinc -fno-stack-protector -m32 -c -D X86 -D __INTEL -mtune=i686 -march=i386
LDFLAGS=-r --oformat=elf32-i386 -melf_i386

AS=nasm

ASFLAGS=-felf -D X86 -I../../include/

.PHONY: all
all: kmain.o boot.o

nanoboot.o: 
	$(LD) $(LDFLAGS) *.o -o nanoboot.o

kmain.o: kmain.c
	$(CC) $(CFLAGS) $(FLAGS) kmain.c

boot.o: boot.asm
	$(AS) $(ASFLAGS) $(FLAGS) boot.asm

.PHONY: clean
clean:
	#rm -v *.o