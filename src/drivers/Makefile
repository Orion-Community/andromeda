include ../scripts/makeIncl
CFLAGS+=-I../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

# TARGETS
TARGETS=text.o net.o kbd.o disk.o pci.o

.PHONY: all
all: drivers.o
	

drivers.o: $(TARGETS)
	$(LD) $(LDFLAGS) -o drivers.o $(TARGETS)
	mv -fv drivers.o ../

graphics.o:
	$(MAKE) -C graphics/ $(MAKEEND)

text.o:
	$(MAKE) -C text/ $(MAKEEND)

net.o:
	$(MAKE) -C net/ $(MAKEEND)

kbd.o:
	$(CC) $(CFLAGS) $(FLAGS) kbd.c

disk.o:
	$(MAKE) -C disk/ $(MAKEEND)
	mv -v disk/disk.o ./

pci.o:
	$(MAKE) -C pci/ $(MAKEEND)

.PHONY: clean
clean:
	$(MAKE) -C graphics/ clean
	rm -fv *.o
