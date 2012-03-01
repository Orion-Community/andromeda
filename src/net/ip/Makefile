# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../../include/" $(FLAGS)
CC=gcc
CCFLAGS=-I"../../include/" -c -m32 -nostdlib -nodefaultlibs -fno-stack-protector \
-finline-functions-called-once -fno-builtin -nostdinc $(FLAGS)
MAKE=make
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

#targets
IP=ip.o
TARGET=arp.o ip_.o

all: $(IP)

$(IP): $(TARGET)
	$(LD) $(LDFLAGS) -o $(IP) $(TARGET)
	mv -vf $(IP) ../

arp.o: arp.c
	$(CC) $(CCFLAGS) -o arp.o arp.c

ip_.o:
	$(CC) $(CCFLAGS) -o ip_.o ip.c
