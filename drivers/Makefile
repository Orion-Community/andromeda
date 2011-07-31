include ../make/makeIncl

LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386
TARGETS=
CONDITIONAL=GRAPHICS:graphics.o INTERRUPTS:interrupts.o
TARGETS+=$(call test,CONDITIONAL)

.PHONY: all
all: drivers.o

drivers.o: $(TARGETS)
ifneq ("$(strip $(TARGETS))","")
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
