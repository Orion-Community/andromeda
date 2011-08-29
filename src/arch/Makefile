# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"include/" $(FLAGS)
CC=gcc
CCFLAGS=-I"../../../include/" -c -m32 -nostdlib -nodefaultlibs -fno-stack-protector \
-finline-functions-called-once -fno-weak -fno-builtin -nostdinc $(FLAGS)
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386
MAKE=make

.PHONY: all
all: x86

.PHONY: x86
x86:
	$(MAKE) FLAGS="$(FLAGS)" -C x86
	mv -v x86/arch.o ./

.PHONY: clean
clean:
	$(MAKE) -C x86 clean

