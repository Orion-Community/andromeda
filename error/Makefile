# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"include/" $(FLAGS)
CC=gcc
CCFLAGS=-I"../include/" -c -m32 -nostdlib -nodefaultlibs -fno-stack-protector -fno-builtin -nostdinc $(FLAGS)
MAKE=make
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

# targets
ERROR=error.o
ERROR_DEPS=panic.o

# source files
PANIC_SRC=panic.c

.PHONY: all
all: $(ERROR)

$(ERROR): $(ERROR_DEPS)
	$(LD) $(LDFLAGS) $(ERROR_DEPS) -o $(ERROR)

panic.o:
	$(CC) $(CCFLAGS) $(PANIC_SRC) -o panic.o

.PHONY: clean
clean:
	rm -v *.o

