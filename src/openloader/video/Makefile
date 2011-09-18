# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"include/" $(FLAGS)
CC=gcc
CCFLAGS=-I"../include" -c -m32 -nostdlib -nodefaultlibs -fno-stack-protector \
-finline-functions-called-once -fno-weak -fno-builtin -nostdinc $(FLAGS)
MAKE=make
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

# targets
VIDEO=video.o
#subtargets
VIDEO_DEPS=vga.o #vidmode.o

#sources
VGA_SRC=vga.c

.PHONY: all
all: $(VIDEO)

$(VIDEO): $(VIDEO_DEPS)
	$(LD) $(LDFLAGS) $(VIDEO_DEPS) -o $(VIDEO)

vga.o:
	$(CC) $(CCFLAGS) $(VGA_SRC) -o vga.o

.PHONY: clean
clean:
	rm -v *.o

