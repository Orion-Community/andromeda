## -*- Makefile -*-
##
## User: michel
## Time: Aug 29, 2011 11:21:33 AM
## Makefile created by Oracle Solaris Studio.
##
##

# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../../../../include/" $(FLAGS)
CC=gcc
CCFLAGS=-I"../../../../include/" -c -m32 -nostdlib -nodefaultlibs -fno-stack-protector \
-finline-functions-called-once -fno-builtin -nostdinc $(FLAGS)
MAKE=make
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

## local sources
CPU=cpu.c
#CPU_INTEL=cpu_intel.asm

## targets
INTEL=i386.o
INTEL_DEPS=cpu.o #cpu_intel.o

all: $(INTEL)

## target: i386.o
$(INTEL): $(INTEL_DEPS)
	$(LD) $(LDFLAGS) -o $(INTEL) $(INTEL_DEPS)

## Target: cpu.o
cpu.o: $(CPU)
	$(CC) $(CCFLAGS) -o cpu.o $(CPU)

cpu_intel.o: $(CPU_INTEL)
	$(AS) $(ASFLAGS) -o cpu_intel.o $(CPU_INTEL)
#### Clean target deletes all generated files ####
clean:
	rm -rfv *.o
