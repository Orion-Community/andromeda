CC=gcc
CFLAGS=-I../../include/ -Iinclude/ -nostdlib -fno-builtin -nostdinc -fno-stack-protector -m32 -c -D X86 -D __INTEL -mtune=i686 -march=i386
ASM=nasm
MAKE=make

LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

INTERRUPTS=interrupts/APIC.c interrupts/IDT.c interrupts/irq.c interrupts/isr.c interrupts/PIC.c
MM=mm/GDT.c

.PHONY: all
all: arch.o

arch.o: $(INTERRUPTS) $(MM)
	$(CC) $(CFLAGS) $(FLAGS) */*.c
	$(MAKE) FLAGS="$(FLAGS)" -C asm
	$(LD) -o arch.o asm/*.o *.o $(LDFLAGS)

.PHONY: clean
clean:
	$(MAKE) -C asm clean
	rm -v *.o
