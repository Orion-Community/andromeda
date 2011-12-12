# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../include/" $(FLAGS)
CC=gcc
CCFLAGS=-I"../include/" -c -m32 -nostdlib -nodefaultlibs -fno-stack-protector \
-finline-functions-called-once -fno-builtin -nostdinc $(FLAGS)
MAKE=make
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

#targets
NET=networking.o
TARGET=arp.o


$(NET): $(TARGET)
	$(LD) $(LDFLAGS) -o $(NET) $(TARGET)
	mv -vf $(NET) ../

arp.o: arp.c
	$(CC) $(CCFLAGS) arp.c -o arp.o