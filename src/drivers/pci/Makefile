include ../../scripts/makeIncl
CFLAGS+=-I../../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../../include/" $(FLAGS)

# targets

PCI=pci.o
PCI_D=pci_.o msi.o

#sources

PCI_=pci.c
MSI=msi.c

.PHONY: all
all: $(PCI)

$(PCI): $(PCI_D)
	$(LD) $(LDFLAGS) $(PCI_D) -o $(PCI)
	mv -fv ./$(PCI) ../$(PCI)
	
pci_.o: $(PCI_)
	$(CC) $(CFLAGS) $(FLAGS) $(PCI_) -o pci_.o

msi.o: $(MSI)
	$(CC) $(CFLAGS) $(FLAGS) $(MSI) -o msi.o

.PHONY: clean
clean:
	rm -vf *.o
