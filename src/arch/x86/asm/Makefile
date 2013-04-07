include ../../../scripts/makeIncl
AS=nasm

ASFLAGS=-felf32 -D X86 -I../../../../include/ $(FLAGS)

#CC=gcc
#CCFLAGS=-I"../../../include/" -c -m32 -nostdlib -nodefaultlibs -fno-stack-protector \
# -fno-builtin -nostdinc -finline-functions-called-once $(FLAGS)

CCFLAGS:=$(CFLAGS) $(FLAGS) -I"../../../../include"

IO=io.asm
IO_INLINE=io.c
IRQ=irq.asm
ISR=isr.asm
GDT=lgdt.asm
ASM_=asm.asm

ASM=io.o io_inline.o lgdt.o asm_.o stdlib.o
OUT=asm.o

LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

.PHONY: all
all: $(ASM)
	$(LD) $(LDFLAGS) $(ASM) -o $(OUT)

io.o: io.asm
	$(AS) $(ASFLAGS) $(FLAGS) $(IO)

io_inline.o: io.c
	$(CC) $(CCFLAGS) $(FLAGS) $(IO_INLINE) -o io_inline.o
#irq.o: irq.asm
#	$(AS) $(ASFLAGS) $(FLAGS) $(IRQ)

#isr.o: isr.asm
#	$(AS) $(ASFLAGS) $(FLAGS) $(ISR)

lgdt.o: lgdt.asm
	$(AS) $(ASFLAGS) $(FLAGS) $(GDT)

asm_.o: asm.asm
	$(AS) $(ASFLAGS) $(FLAGS) $(ASM_) -o asm_.o

stdlib.o: stdlib.asm
	$(AS) $(ASFLAGS) stdlib.asm -o stdlib.o

.PHONY: clean
clean:
	rm -fv *.o
