AS=nasm

ASFLAGS=-felf -D X86 -I../../../include/

IO=io.asm
IRQ=irq.asm
ISR=isr.asm
GDT=lgdt.asm
ASM_=asm.asm

ASM=io.o lgdt.o asm_.o
OUT=asm.o

LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

.PHONY: all
all: $(ASM)
	$(LD) $(LDFLAGS) $(ASM) -o $(OUT)

io.o: io.asm
	$(AS) $(ASFLAGS) $(FLAGS) $(IO)

#irq.o: irq.asm
#	$(AS) $(ASFLAGS) $(FLAGS) $(IRQ)

#isr.o: isr.asm
#	$(AS) $(ASFLAGS) $(FLAGS) $(ISR)

lgdt.o: lgdt.asm
	$(AS) $(ASFLAGS) $(FLAGS) $(GDT)

asm_.o: asm.asm
	$(AS) $(ASFLAGS) $(FLAGS) $(ASM_) -o asm_.o

.PHONY: clean
clean:
	rm -fv *.o
