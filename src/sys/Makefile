# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"include/" $(FLAGS)
CC=gcc
CCFLAGS=-I"../include" -c -m32 -nostdlib -nodefaultlibs -fno-stack-protector -fno-builtin -nostdinc $(FLAGS)
MAKE=make
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

# targets
SYS=sys.o
#subtargets
SYS_DEPS=stdlib.o io.o

# source files
IO_SRC=io.asm
STDLIB_SRC=stdlib.asm

#headers
STDLIB_HDR=stdlib.asmh

.PHONY: all
all: $(SYS_DEPS)
	$(LD) $(LDFLAGS) $(SYS_DEPS) -o $(SYS)

stdlib.o:
	$(AS) $(ASFLAGS) $(STDLIB_SRC) -o stdlib.o

io.o:
	$(AS) $(ASFLAGS) $(IO_SRC) -o io.o

.PHONY: clean
clean:
	rm -v *.o

