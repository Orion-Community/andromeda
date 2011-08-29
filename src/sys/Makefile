# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../include/" $(FLAGS)
CC=gcc
CCFLAGS=-I"../include" -c -m32 -nostdlib -nodefaultlibs -fno-stack-protector \
-finline-functions-called-once -fno-weak -fno-builtin -nostdinc $(FLAGS)
MAKE=make
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

# targets
SYS=sys.o
#subtargets
SYS_DEPS=stdlib.o io.o keyboard.o dev.o disk.o

# source files
IO_SRC=io.asm
STDLIB_SRC=stdlib.asm
KB_SRC=keyboard.c
PS2_SRC=ps2controller.asm


.PHONY: all
all: $(SYS_DEPS)
	$(LD) $(LDFLAGS) $(SYS_DEPS) -o $(SYS)

stdlib.o:
	$(AS) $(ASFLAGS) $(STDLIB_SRC) -o stdlib.o

io.o:
	$(AS) $(ASFLAGS) $(IO_SRC) -o io.o

keyboard.o:
	$(CC) $(CCFLAGS) $(KB_SRC) -o keyboard.o

dev.o:
	$(MAKE) -C dev/ all
	mv -v dev/dev.o ./
	
disk.o:
	$(MAKE) -C disk/ all
	mv -v disk/disk.o ./

.PHONY: clean
clean:
	$(MAKE) -C dev/ clean
	$(MAKE) -C disk/ clean
	rm -v *.o
