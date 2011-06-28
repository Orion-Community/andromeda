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

.PHONY: assembly.o
assembly.o: io.o irq.o isr.o lgdt.o asm.o
	$(LD) -o assembly.o *.o $(LDFLAGS)
	mv assembly.o ../

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
