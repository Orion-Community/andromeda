#
# Sched Makefile
#

include ../../scripts/makeIncl
CFLAGS+=-I../../../include
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

CSRCS=sched.c cfs.c thread.c task.c
COBJS=$(CSRCS:.c=.o)

# TARGETS
TARGET=sched.o

.PHONY: all clean

all: $(COBJS)
	$(LD) $(LDFLAGS) -o_$(TARGET) $(COBJS)
	mv _$(TARGET) ../$(TARGET)

clean:
	rm -Rfv $(COBJS)

