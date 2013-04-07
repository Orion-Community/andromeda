# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../../../include/" $(FLAGS)
CC=gcc
CFLAGS=-I"../../../include/" -c -m32 -nostdlib -nodefaultlibs -fno-stack-protector \
-finline-functions-called-once -fno-builtin -nostdinc $(FLAGS)
MAKE=make
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

#targets
IP=ip.o
TARGETS=arp.o ip.o

all: ip

ip: $(TARGETS)
	$(LD) $(LDFLAGS) -o ../$(IP) $(TARGETS)

%o: %c $(INCLUDES)
	$(CC) $(CFLAGS) $(FLAGS) $< -o $@

.PHONY: clean
clean:
	rm -fv *.o
