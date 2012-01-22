## -*- Makefile -*-
##
## User: michel
## Time: Jan 22, 2012 3:27:57 PM
## Makefile created by Oracle Solaris Studio.
##
##

# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../../include/" $(FLAGS)
CC=gcc
CCFLAGS=-I"../../include/" -c -m32 -nostdlib -nodefaultlibs -fno-stack-protector \
-finline-functions-called-once -fno-builtin -nostdinc $(FLAGS)
MAKE=make
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

TARGETS=net.o
all: core.o

## Target: core.o
core.o: net.o
	$(LD) $(LDFLAGS) -o core.o $(TARGETS)
	mv -vf core.o ../

## Target: net.o
net.o: net.c
	$(CC) $(CCFLAGS) -o net.o net.c


#### Clean target deletes all generated files ####
clean:


