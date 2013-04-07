# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../../include/" $(FLAGS)
CC=gcc
CCFLAGS=-I"../../include/" -c -m32 -nostdlib -nodefaultlibs -fno-stack-protector \
-finline-functions-called-once -fno-builtin -nostdinc $(FLAGS)
MAKE=make
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

#targets
NET=net.o
TARGET=core.o ip.o ethernet.o netlayer.o

.PHONY: all core.o ip.o ethernet.o
all: $(NET)

$(NET): $(TARGET)
	$(LD) $(LDFLAGS) -o $(NET) $(TARGET)
	mv -vf $(NET) ../

core.o:
	$(MAKE) -C core/ $(MAKEEND)

ip.o:
	$(MAKE) -C ip/ $(MAKEEND)

ethernet.o:
	$(MAKE) -C eth/ $(MAKEEND)

netlayer.o:
	$(CC) $(CCFLAGS) -o netlayer.o netlayer.c
