include ../../scripts/makeIncl
CFLAGS+=-I../../../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../../../include/" $(FLAGS)

# targets

PCI=pci.o
TARGETS=pci.o msi.o

.PHONY: all
all: pci

pci: $(TARGETS)
	$(LD) $(LDFLAGS) $(TARGETS) -o ../$(PCI)

%o: %c
	$(CC) $(CFLAGS) $(FLAGS) $< -o $@

.PHONY: clean
clean:
	rm -vf *.o
