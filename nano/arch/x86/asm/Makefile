AS=nasm

ASFLAGS=-felf -D X86 -I../../../include/

IO=io.asm
IRQ=irq.asm
ISR=isr.asm
GDT=lgdt.asm
ASM=asm.asm
BOOT=boot.asm

LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

.PHONY: all
all: io.o irq.o isr.o lgdt.o asm.o boot.o

io.o: io.asm
	$(AS) $(ASFLAGS) $(IO)

irq.o: irq.asm
	$(AS) $(ASFLAGS) $(IRQ)

isr.o: isr.asm
	$(AS) $(ASFLAGS) $(ISR)

lgdt.o: lgdt.asm
	$(AS) $(ASFLAGS) $(GDT)

asm.o: asm.asm
	$(AS) $(ASFLAGS) $(ASM)

boot.o: boot.asm
	$(AS) $(ASFLAGS) $(BOOT)

.PHONY: clean
clean:
	rm -v *.o
