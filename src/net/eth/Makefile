# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../../../include/" $(FLAGS)
CC=gcc
CCFLAGS=-I"../../../include/" -c -m32 -nostdlib -nodefaultlibs -fno-stack-protector \
-finline-functions-called-once -fno-builtin -nostdinc $(FLAGS)
MAKE=make
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

#targets
ETH=ethernet.o
TARGET=eth.o

all: $(ETH)

$(ETH): $(TARGET)
	$(LD) $(LDFLAGS) -o $(ETH) $(TARGET)
	mv -vf $(ETH) ../

eth.o:
	$(CC) $(CCFLAGS) -o eth.o eth.c
