# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../../include/" $(FLAGS)
CC=gcc
CCFLAGS=-I"../../include" -c -m32 -nostdlib -nodefaultlibs -fno-stack-protector -fno-builtin -nostdinc $(FLAGS)
MAKE=make
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

# targets

DEV=dev.o
DEV_D=ps2.o

#sources

PS2=ps2.c

.PHONY: all
all: $(DEV)

$(DEV): $(DEV_D)
	$(LD) $(LDFLAGS) $(DEV_D) -o $(DEV)

ps2.o: $(PS2)
	$(CC) $(CCFLAGS) $(PS2) -o ps2.o
	
.PHONY: clean
clean:
	rm -v *.o