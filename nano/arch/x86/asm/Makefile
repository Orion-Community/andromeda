AS=nasm

ASFLAGS=-felf -D X86 -I../../../include/

IO=io.asm
IRQ=irq.asm
ISR=isr.asm
GDT=lgdt.asm
ASM=asm.asm

LD=ld
LDFLAGS=--allow-shlib-undefined -shared -nostdlib --oformat=elf32-i386 -melf_i386

.PHONY: all
all: assembly.o

assembly.o: io.o irq.o isr.o lgdt.o asm.o
#	$(LD) -o assembly.o *.o $(LDFLAGS)

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

.PHONY: clean
clean:
	rm -v *.o
