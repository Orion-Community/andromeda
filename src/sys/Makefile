include ../scripts/makeIncl
CFLAGS+=-I../../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../include/" $(FLAGS)

# targets
SYS=sys.o
#subtargets
SYS_DEPS=sys_.o

PS2_SRC=ps2controller.asm
SYS_SRC=sys.c


.PHONY: all
all: $(SYS_DEPS)
	$(LD) $(LDFLAGS) $(SYS_DEPS) -o $(SYS)

sys_.o:
	$(CC) $(CFLAGS) $(FLAGS) $(SYS_SRC) -o sys_.o

.PHONY: clean
clean:
	rm -fv *.o
