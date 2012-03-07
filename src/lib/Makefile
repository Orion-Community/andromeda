include ../scripts/makeIncl

## -*- Makefile -*-
##
## User: Michel Megens
## Time: 24th of February
## Makefile created by Michel Megens.
##
##

# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../include/" $(FLAGS)
INCDIR=-I../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386
MAKE_FLAGS=FLAGS="$(FLAGS) "
TARGET=lib.o

OBJS=list.o

.PHONY: all
all: $(TARGET)
	-mv $(TARGET) ../

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $(TARGET) $(OBJS)

.PHONY: list.o
list.o:
	$(MAKE) -C list $(MAKE_FLAGS)
