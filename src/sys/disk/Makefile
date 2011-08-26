## -*- Makefile -*-
##
## User: michel
## Time: Aug 24, 2011 3:05:15 PM
## 
##
##
##

AS=nasm
ASFLAGS=-felf32 -I"../../include/" $(FLAGS)
CC=gcc
CCFLAGS=-I"../../include" -c -m32 -nostdlib -nodefaultlibs -fno-stack-protector -fno-builtin -nostdinc $(FLAGS)
MAKE=make
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

## Sources
ATA=ata.c
DISK=ata.o

.PHONY: all
all: disk.o

## Target: ata.o
ata.o: ata.c
	$(CC) $(CCFLAGS) $(ATA) -o ata.o

## Target: disk.o
disk.o: ata.o
	$(LD) $(LDFLAGS) $(DISK) -o disk.o

#### Clean target deletes all generated files ####
.PHONY: clean
clean:
	rm -v *.o

# Enable dependency checking
.KEEP_STATE:
.KEEP_STATE_FILE:.make.state.GNU-amd64-Linux

