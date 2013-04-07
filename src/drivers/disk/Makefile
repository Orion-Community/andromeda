## -*- Makefile -*-
##
## User: michel
## Time: Aug 24, 2011 3:05:15 PM
##
##
##
##

include ../../scripts/makeIncl
CFLAGS+=-I../../../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../../../include/" $(FLAGS)

## Sources
ATA=ata.c
DISK=ata.o

.PHONY: all
all: disk.o

%o: %c
	$(CC) $(CFLAGS) $(FLAGS) $< -o $@

## Target: disk.o
disk.o: ata.o
	$(LD) $(LDFLAGS) $(DISK) -o disk.o

#### Clean target deletes all generated files ####
.PHONY: clean
clean:
	rm -fv *.o

# Enable dependency checking
.KEEP_STATE:
.KEEP_STATE_FILE:.make.state.GNU-amd64-Linux

