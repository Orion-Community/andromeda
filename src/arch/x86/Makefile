# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"include/" $(FLAGS)
CC=gcc
CCFLAGS=-I"../../../include/" -c -m32 -nostdlib -nodefaultlibs -fno-stack-protector \
-finline-functions-called-once -fno-weak -fno-builtin -nostdinc $(FLAGS)
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386
MAKE=make

# targets
ARCH=arch.o

.PHONY: all
all: $(ARCH)

$(ARCH):
	$(MAKE) FLAGS="$(FLAGS)" -C disk
	$(MAKE) FLAGS="$(FLAGS)" -C kernel
	$(MAKE) FLAGS="$(FLAGS)" -C mm
	mv kernel/kernel.o ./
	mv disk/disk.o ./
	mv mm/mm.o ./
	$(LD) $(LDFLAGS) -o $(ARCH) *.o

.PHONY: clean
clean:
	$(MAKE) -C disk clean
	$(MAKE) -C kernel clean
	$(MAKE) -C mm clean
	rm -rfv *.o
