# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../../../../include/" $(FLAGS)
CC=gcc
CCFLAGS=-I"../../../../include/" -c -m32 -nostdlib -nodefaultlibs -fno-stack-protector \
-finline-functions-called-once -fno-builtin -nostdinc $(FLAGS)
MAKE=make
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

## TARGETS
APIC_=apic_.o

APIC=apic.o

## sources
APIC_S=apic.c

## deps
APIC_DEPS=$(APIC_)

.PHONY: all
all: $(APIC_DEPS)
	$(LD) $(LDFLAGS) $(APIC_DEPS) -o $(APIC)

$(APIC_): $(APIC_S)
	$(CC) $(CCFLAGS) $(APIC_S) -o $(APIC_)

.PHONY: clean
clean:
	rm -rfv *.o
