AS=nasm
ASFLAGS=-felf32 -I"../../../include/" $(FLAGS)
CC=gcc
CCFLAGS=-I"../../../include/" -c -m32 -nostdlib -nodefaultlibs -fno-stack-protector \
 -fno-builtin -nostdinc -finline-functions-called-once $(FLAGS)
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

TASK=task.o
KERN=kern.o

TASK_SRC=task.c

KERNEL_DEPS = $(TASK)


.PHONY: all
all: $(KERN)

$(KERN): $(KERNEL_DEPS)
	$(LD) $(LDFLAGS) $(KERNEL_DEPS) -o $(KERN)

$(TASK): $(TASK_SRC)
	$(CC) $(CCFLAGS) $(TASK_SRC) -o $(TASK)