include ../scripts/makeIncl
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386
TARGETS=
CONDITIONAL=GRAPHICS:graphics.o BEEP:beep.o
TARGETS+=$(call test,CONDITIONAL)
CFLAGS+= -I../include

.PHONY: all
all: drivers.o

drivers.o: $(TARGETS)
ifneq ("$(strip $(TARGETS))","")
	$(LD) $(LDFLAGS) -o drivers.o *.o
	mv -fv drivers.o ../
endif	

graphics.o:
	$(MAKE) -C graphics/ $(MAKEEND)

beep.o:
	$(MAKE) -C system\ beep/ $(MAKEEND)

.PHONY: clean
clean:
	$(MAKE) -C graphics/ clean
	$(MAKE) -C system\ beep/ clean
	rm -fv *.o
