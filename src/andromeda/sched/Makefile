#
# Sched submodule Maker
#

include ../../scripts/makeIncl
CFLAGS+=-I../../include
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

SCHED_SRCS=sched-core.c sched.c thread.c task.c
SCHED_OBJS=$(SCHED_SRCS:.c=.o)

# TARGET
SCHED_TARGET=sched.o

all: $(SCHED_OBJS)
	$(LD) $(LDFLAGS) -o_$(SCHED_TARGET) $(SCHED_OBJS)
	-mv _$(TARGET) ../$(TARGET)