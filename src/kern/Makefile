# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"include/" $(FLAGS)
CC=gcc
CCFLAGS=-I"../include/" -c -m32 -nostdlib -nodefaultlibs -fno-stack-protector -fno-builtin -nostdinc $(FLAGS)
MAKE=make
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

# targets
KERN=kern.o

# source files
KERN_SRC=kmain.c

.PHONY: all
all: $(KERN)

$(KERN): $(KERN_SRC)
	$(CC) $(CCFLAGS) -o $(KERN) $(KERN_SRC)

.PHONY: clean
clean:
	#rm -v *.o

