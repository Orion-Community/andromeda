include ../scripts/makeIncl
CFLAGS+=-I../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../include/" $(FLAGS)

# targets
SYS=sys.o
#subtargets
SYS_DEPS=stdlib.o keyboard.o dev.o disk.o sys_.o

# source files
STDLIB_SRC=stdlib.asm
KB_SRC=keyboard.c
PS2_SRC=ps2controller.asm
SYS_SRC=sys.c


.PHONY: all
all: $(SYS_DEPS)
	$(LD) $(LDFLAGS) $(SYS_DEPS) -o $(SYS)

stdlib.o:
	$(AS) $(ASFLAGS) $(STDLIB_SRC) -o stdlib.o

keyboard.o:
	$(CC) $(CFLAGS) $(FLAGS) $(KB_SRC) -o keyboard.o

dev.o:
	$(MAKE) -C dev/ $(MAKEEND)
	mv -v dev/dev.o ./
	
disk.o:
	$(MAKE) -C disk/ $(MAKEEND)
	mv -v disk/disk.o ./

sys_.o:
	$(CC) $(CFLAGS) $(FLAGS) $(SYS_SRC) -o sys_.o

.PHONY: clean
clean:
	$(MAKE) $(MAKEEND) -C dev/ clean
	$(MAKE) $(MAKEEND) -C disk/ clean
	rm -fv *.o
