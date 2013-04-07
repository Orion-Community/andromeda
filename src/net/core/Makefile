## -*- Makefile -*-
##
## User: michel
## Time: Jan 22, 2012 3:27:57 PM
## Makefile created by Michel Megens.
##
##

# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../../../include/" $(FLAGS)
CC=gcc
CFLAGS=-I"../../../include/" -c -m32 -nostdlib -nodefaultlibs -fno-stack-protector \
-finline-functions-called-once -fno-builtin -nostdinc -O0 $(FLAGS)
MAKE=make
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

# source files
SRC=net.c
# target object files
TARGET=core.o
OBJS=$(SRC:.c=.o)

all: $(TARGET)
	-mv -v $(TARGET) ../

## Target: core.o
$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJS)

%o: %c
	$(CC) $(CFLAGS) $< -o $@


#### Clean target deletes all generated files ####
clean:
	-rm -Rv `find -name "*.o"`
