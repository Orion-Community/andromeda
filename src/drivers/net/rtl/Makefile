AS=nasm
ASFLAGS=-felf32 -I"../../../../include/" $(FLAGS)
CC=gcc
CCFLAGS=-I"../../../../include/" -c -m32 -nostdlib -nodefaultlibs -fno-stack-protector \
-finline-functions-called-once -fno-builtin -nostdinc $(FLAGS)
MAKE=make
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

OUT=rtl.o

TARGET=rtl8168.o

$(OUT): $(TARGET)
	$(LD) $(LDFLAGS) -o $(OUT) $(TARGET)
	mv -vf $(OUT) ../

rtl8168.o: rtl8168.c
	$(CC) $(CCFLAGS) rtl8168.c -o rtl8168.o
