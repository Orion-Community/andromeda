MAKEEND=FLAGS="$(FLAGS) " DEFS="$(DEFS)" $(DEFS)
CC=gcc
CFLAGS=-I../../../include/ -nostdlib -fno-builtin -nostdinc -fno-stack-protector -m32 -c -D X86 -D __INTEL -mtune=i686 -march=i386
ASM=nasm
MAKE=make

LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

INTERRUPTS=interrupts/APIC.c interrupts/IDT.c interrupts/irq.c interrupts/isr.c interrupts/PIC.c
MM=mm/GDT.c

.PHONY: all
all: arch.o
	mv -fv arch.o ../

arch.o: $(INTERRUPTS) $(MM)
	$(CC) $(CFLAGS) $(FLAGS) */*.c
	$(MAKE) -C asm $(MAKEEND)
	$(LD) -o arch.o asm/*.o *.o $(LDFLAGS)

.PHONY: clean
clean:
	$(MAKE) -C asm clean
	rm -fv *.o
