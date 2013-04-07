include ../../scripts/makeIncl
MAKEEND=FLAGS="$(FLAGS) " DEFS="$(DEFS)" $(DEFS)
CFLAGS+=-I../../../include/

# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../../../include/" $(FLAGS)
#CC=gcc
#CCFLAGS=-I"../../include/" -c -m32 -nostdlib -nodefaultlibs -fno-stack-protector \
#-finline-functions-called-once -fno-builtin -nostdinc -O0 $(FLAGS)

CCFLAGS:=$(CFLAGS) -I"../../include" $(FLAGS)
#MAKE=make
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

## TARGETS
X86=x86.o

## DEPS
DEPS=kernel.o mm.o asm.o boot.o byteorder.o

.PHONY: all
all: x86.o


x86.o: $(DEPS)
	$(LD) -o $(X86) $(DEPS) $(LDFLAGS)

.PHONY: mm.o kernel.o boot.o asm.o
mm.o:
	$(MAKE) $(MAKEEND) -C mm/
	mv -fv mm/mm.o ./

kernel.o:
	$(MAKE) $(MAKEEND) -C kernel/
	mv kernel/kernel.o ./

boot.o:
	$(MAKE) $(MAKEEND) -C boot/
	mv boot/boot.o ./

asm.o:
	$(MAKE) -C asm $(MAKEEND)
	mv -fv asm/asm.o ./

byteorder.o: byteorder.c
	$(CC) $(CCFLAGS) -o byteorder.o byteorder.c
