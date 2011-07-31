MAKEEND=FLAGS="$(FLAGS) " DEFS="$(DEFS)" $(DEFS)
MAKE=make
LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386
DRIVERS=
ifeq ($(GRAPHICS),enabled)
	DRIVERS+=graphics.o
endif
ifeq ($(INTERRUPTS),enabled)
	DRIVERS+=interrupts.o
endif

.PHONY: all
all: drivers.o

drivers.o: $(DRIVERS)
ifneq ($(DRIVERS),)
	$(LD) $(LDFLAGS) -o drivers.o *.o
	mv -v drivers.o ../
endif

graphics.o:
	$(MAKE) -C graphics/ $(MAKEEND)

interrupts.o:
	$(MAKE) -C interrupts/ $(MAKEEND)

.PHONY: clean
clean:
	$(MAKE) -C graphics/ clean
#	$(MAKE) -C interrupts/ clean
	rm -fv *.o
