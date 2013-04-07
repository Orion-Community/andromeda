## -*- Makefile -*-
##
## User: michel
## Time: Sep 5, 2011 10:39:01 AM
## Makefile created by Oracle Solaris Studio.
##
## This file is generated automatically.
##

include ../../../../scripts/makeIncl

# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../../../../../include/" $(FLAGS)
#CC=gcc
#CCFLAGS=-I"../../../../include/" -c -m32 -nostdlib -nodefaultlibs -fno-stack-protector \
# -fno-builtin -nostdinc -finline-functions-called-once $(FLAGS)
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

CCFLAGS:=$(CFLAGS) -I"../../../../../include" $(FLAGS)

all: acpi.o acpi_.o

ACPI=acpi.c
ACPI_=acpi_.o

## Target: acpi.o
acpi.o: acpi_.o
	$(LD) $(LDFLAGS) $(ACPI_) -o acpi.o

## Target: acpi_.o
acpi_.o: acpi.c
	$(CC) $(CCFLAGS) $(ACPI) -o acpi_.o

#### Clean target deletes all generated files ####
clean:
	rm -vf *.o
