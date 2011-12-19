include ../scripts/makeIncl
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386
TARGETS=text.o net.o

.PHONY: all
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

.PHONY: clean
clean:
	$(MAKE) -C graphics/ clean
	rm -fv *.o
