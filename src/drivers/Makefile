include ../scripts/makeIncl
CFLAGS+=-I../../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

# TARGETS
TARGETS=text.o net.o kbd.o disk.o pci.o root.o virtual.o legacy.o timer.o

.PHONY: all graphics.o text.o net.o kbd.o disk.o pci.o virtual.o legacy.o timer.o
all: drivers.o


drivers.o: $(TARGETS)
	$(LD) $(LDFLAGS) -o drivers.o *.o
	mv -fv drivers.o ../

graphics.o:
	$(MAKE) -C graphics/ $(MAKEEND)

text.o:
	$(MAKE) -C text/ $(MAKEEND)

net.o:
	$(MAKE) -C net/ $(MAKEEND)

kbd.o:
	$(MAKE) -C keyboard/ $(MAKEEND)

disk.o:
	$(MAKE) -C disk/ $(MAKEEND)
	mv -v disk/disk.o ./

pci.o:
	$(MAKE) -C pci/ $(MAKEEND)

root.o: root.c
	$(CC) $(CFLAGS) $(FLAGS) root.c

virtual.o:
	$(MAKE) -C virtual/ $(MAKEEND)

legacy.o:
	$(MAKE) -C legacy/ $(MAKEEND)

timer.o:
	$(MAKE) -C timer/ $(MAKEEND)

.PHONY: clean
clean:
	$(MAKE) -C graphics/ clean
	rm -fv *.o
