include ../../scripts/makeIncl
CFLAGS+=-I../../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../../include/" $(FLAGS)

# targets

DEV=dev.o
DEV_D=pci.o ps2.o

#sources

PS2=ps2.c
PCI=pci.c

.PHONY: all
all: $(DEV)

$(DEV): $(DEV_D)
	$(LD) $(LDFLAGS) $(DEV_D) -o $(DEV)

ps2.o: $(PS2)
	$(CC) $(CFLAGS) $(FLAGS) $(PS2) -o ps2.o
	
pci.o: $(PCI)
	$(CC) $(CFLAGS) $(FLAGS) $(PCI) -o pci.o
	
.PHONY: clean
clean:
	rm -vf *.o
