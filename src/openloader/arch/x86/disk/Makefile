AS=nasm
ASFLAGS=-felf32 -I"../../../include/" $(FLAGS)
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

# targets
IDE=ide.o
DISK=disk.o

#sources
IDE_SRC=ide.asm
DISK_DEPS=ide.o

.PHONY: all
all: $(DISK_DEPS)
	$(LD) $(LDFLAGS) $(DISK_DEPS) -o $(DISK)

$(IDE): $(IDE_SRC)
	$(AS) $(ASFLAGS) $(IDE_SRC) -o $(IDE)

.PHONY: clean
clean:
	rm -v *.o
