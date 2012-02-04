include ../../scripts/makeIncl
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

TARGETS=rtl.o

.PHONY: all rtl.o
all: net.o

net.o: $(TARGETS)
	$(LD) $(LDFLAGS) -o net.o *.o
	mv -fv net.o ../

rtl.o:
	$(MAKE) -C rtl/ $(MAKEEND)

.PHONY: clean
clean:
	$(MAKE) -C graphics/ clean
	rm -fv *.o
