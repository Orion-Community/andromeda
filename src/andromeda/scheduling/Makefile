include ../../scripts/makeIncl
CFLAGS+=-I../../../include
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

ELF=elf.c exec.c
TIMER=timer.c
CORE=core.c
SYSCALL=syscall.c
PANIC=panic.c

MK_FLAGS=FLAGS="$(FLAGS)"

TARGET=scheduling.o
SRCS=sched.c roundrobin.c rr_test.c
OBJS=$(SRCS:.c=.o)

.PHONY: all

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) -o$(TARGET) $(OBJS)
	-mv $(TARGET) ../$(TARGET)
