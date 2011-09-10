# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../include/" $(FLAGS)
CC=gcc
CCFLAGS=-I"../include/" -c -m32 -nostdlib -nodefaultlibs -fno-stack-protector \
-finline-functions-called-once -fno-weak -fno-builtin -nostdinc $(FLAGS)
MAKE=make
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

# targets
KERN=main.o i386.o

# source files
MAIN_SRC=kmain.c

.PHONY: all
all: $(KERN)
	$(LD) $(LDFLAGS) -o kern.o $(KERN)

main.o: $(MAIN_SRC)
	$(CC) $(CCFLAGS) -o main.o $(MAIN_SRC)
	
i386.o:
	$(MAKE) -C x86/ all
	mv -fv x86/i386.o ./

.PHONY: clean
clean:
	$(MAKE) -C x86/ clean
	rm -v *.o

