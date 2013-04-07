include ../../../scripts/makeIncl
#CC=gcc
LD=ld
#CFLAGS=-I../../../include/ -nostdlib -fno-builtin -nostdinc \
#-fno-stack-protector -m32 -c -D X86 -D __INTEL -mtune=i686 -march=i386 \
#-D VERSION=\"$(shell git describe)\"
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

CFLAGS+=-I"../../../../include"

AS=nasm

ASFLAGS=-felf -D X86 -I../../include/

.PHONY: all
all: boot.o

boot.o: init.o start.o
	$(LD) $(LDFLAGS) -o boot.o *.o

init.o: init.c
	$(CC) $(CFLAGS) $(FLAGS) init.c

start.o: start.asm
	$(AS) $(ASFLAGS) $(FLAGS) start.asm

.PHONY: clean
clean:
	#rm -fv *.o
