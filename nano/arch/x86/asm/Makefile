AS=nasm

ASFLAGS=-felf -D X86 -I../../../../include/

IO=io.asm
IRQ=irq.asm
ISR=isr.asm
GDT=lgdt.asm
ASM=asm.asm

LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

.PHONY: all
all: io.o irq.o isr.o lgdt.o asm.o

io.o: io.asm
	$(AS) $(ASFLAGS) $(FLAGS) $(IO)

irq.o: irq.asm
	$(AS) $(ASFLAGS) $(FLAGS) $(IRQ)

isr.o: isr.asm
	$(AS) $(ASFLAGS) $(FLAGS) $(ISR)

lgdt.o: lgdt.asm
	$(AS) $(ASFLAGS) $(FLAGS) $(GDT)

asm.o: asm.asm
	$(AS) $(ASFLAGS) $(FLAGS) $(ASM)

.PHONY: clean
clean:
	rm -v *.o
