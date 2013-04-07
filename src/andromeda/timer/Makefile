include ../../scripts/makeIncl
## -*- Makefile -*-
##
## User: Michel Megens
## Time: 23th of February
## Makefile created by Michel Megens.
##
##

##

# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../../../include/" $(FLAGS)
INCDIR=-I../../../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

CFLAGS += $(INCDIR)

TARGET=ktimer.o

# source files..
SRCS=timer.c virtual.c
OBJS=$(SRCS:.c=.o)

all: $(TARGET)
	mv ./$(TARGET) ../$(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

.PHONY: clean
clean:
	-rm -fv $(OBJS)
