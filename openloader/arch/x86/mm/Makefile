# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../../../include/" $(FLAGS)
CC=gcc
CCFLAGS=-I"../../../include/" -c -m32 -nostdlib -nodefaultlibs -fno-stack-protector \
-finline-functions-called-once -fno-weak -fno-builtin -nostdinc $(FLAGS)
MAKE=make
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

# targets
IRQ30=irq30.o
MM=mm.o

# source files
IRQ30_SRC=cIRQ30.asm
MM_DEPS=irq30.o

.PHONY: all
all: $(MM_DEPS)
	$(LD) $(LDFLAGS) $(MM_DEPS) -o $(MM)

$(IRQ30): $(IRQ30_SRC)
	$(AS) $(ASFLAGS) -o $(IRQ30) $(IRQ30_SRC)

.PHONY: clean
clean:
	rm -v *.o

