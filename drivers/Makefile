MAKE=make
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

.PHONY: all
all: drivers.o

drivers.o: graphics.o
	$(LD) $(LDFLAGS) -o drivers.o *.o

graphics.o:
	$(MAKE) -C graphics/
	mv graphics/vga.o ./graphics.o

interrupts.o:
	$(MAKE) -C interrupts/
	mv -v interrupts/interrupts.o ./

.PHONY: clean
clean:
	$(MAKE) -C graphics/ clean
#	$(MAKE) -C interrupts/ clean
	rm *.o