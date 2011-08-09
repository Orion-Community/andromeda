# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"include/" $(FLAGS)
CC=gcc
CCFLAGS=-I"../../../include/" -c -m32 -nostdlib -nodefaultlibs -fno-stack-protector -fno-builtin -nostdinc $(FLAGS)
MAKE=make
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

# targets
IRQ30=irq30.o

# source files
IRQ30_SRC=cIRQ30.asm

# header files
IRQ30_HDR=include/cIRQ30.asmh

.PHONY: all
all: $(IRQ30)

$(IRQ30): $(IRQ30_SRC) $(IRQ30_HDR)
	$(AS) $(ASFLAGS) -o $(IRQ30) $(IRQ30_SRC)

.PHONY: clean
clean:
	rm -v *.o

