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
	$(MAKE) FLAGS="$(FLAGS)" -C diskio
	$(MAKE) FLAGS="$(FLAGS)" -C interrupts
	$(MAKE) FLAGS="$(FLAGS)" -C mm
	$(LD) $(LDFLAGS) -o $(ARCH) */*.o

.PHONY: clean
clean:
	$(MAKE) -C diskio clean
	$(MAKE) -C interrupts clean
	$(MAKE) -C mm clean
