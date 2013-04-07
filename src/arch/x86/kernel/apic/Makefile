#include ../../../../scripts/makeIncl
# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../../../../../include/" $(FLAGS)
CC=gcc
CCFLAGS=-I"../../../../../include/" -c -m32 -nostdlib -nodefaultlibs -fno-stack-protector \
-finline-functions-called-once -fno-builtin -nostdinc $(FLAGS)
#MAKE=make
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386
#CCFLAGS:=$(CFLAGS) $(FLAGS) -I"../../../../include" -finline-functions-called-once -nodefaultlibs -nostdinc -fno-builtin -nostdlib

## TARGETS
APIC_=apic_.o
IOAPIC_=ioapic.o

APIC=apic.o

## sources
APIC_S=apic.c
IOAPIC_S=ioapic.c

## deps
APIC_DEPS=$(APIC_) $(IOAPIC_)

.PHONY: all
all: $(APIC_DEPS)
	$(LD) $(LDFLAGS) $(APIC_DEPS) -o $(APIC)

$(APIC_): $(APIC_S)
	$(CC) $(CCFLAGS) $(APIC_S) -o $(APIC_)

$(IOAPIC_): $(IOAPIC_S)
	$(CC) $(CCFLAGS) $(IOAPIC_S) -o $(IOAPIC_)

.PHONY: clean
clean:
	rm -rfv *.o
