AS=nasm

ASFLAGS=-felf -D X86 -I../../../include/

IO=io.asm
IRQ=irq.asm
ISR=isr.asm
GDT=lgdt.asm
ASM=asm.asm

.PHONY: all
all: out.o
	ld -o out.o *.o --allow-shlib-undefined -shared -nostdlib --oformat=elf32-i386 -melf_i386

out.o: io.o irq.o isr.o lgdt.o asm.o

io.o:
	$(AS) $(ASFLAGS) $(IO)

irq.o:
	$(AS) $(ASFLAGS) $(IRQ)

isr.o:
	$(AS) $(ASFLAGS) $(ISR)

lgdt.o:
	$(AS) $(ASFLAGS) $(GDT)

asm.o:
	$(AS) $(ASFLAGS) $(ASM)

.PHONY: clean
clean:
	rm -v *.o
