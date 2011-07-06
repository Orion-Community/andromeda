include ../make/makeIncl
include ../make/x86

.PHONY: all
all: x86

.PHONY: amd64
amd64:
	@echo "Not yet implemented"

.PHONY: x86
x86: $(OUTC)

$(OUTC):
	$(MAKE) -C error
	$(MAKE) -C mm
	$(MAKE) -C kern
	$(MAKE) -C arch
	$(MAKE) -C text
	$(MAKE) -C nano
	$(MAKE) -C math
	mv -v error/panic.o ./
	mv -v mm/mm.o ./
	mv -v kern/kern.o ./
	mv -v arch/arch.o ./
	mv -v text/text.o ./
	mv -v nano/*.o ./
	mv -v math/maths.o ./
	
	$(LD) $(LDFLAGS) $(LDCOMPRESSED) -o $(OUTC) *.o

.PHONY: clean
clean:
	$(MAKE) -C error clean
	$(MAKE) -C mm clean
	$(MAKE) -C kern clean
	$(MAKE) -C arch clean
	$(MAKE) -C nano clean
	rm -v *.o
	rm $(OUTC)

.PHONY: all
test: all
	qemu -m 600M -kernel $(OUTC)
	
.PHONY: ctest
ctest: clean test

.PHONY: usage
usage:
	@echo "USAGE"
	@echo "Target all (default target)"
	@echo "	Same as x86."
	@echo "Target x86:"
	@echo "	makes for the x86 PC."
	@echo "Target amd64:"
	@echo "	not supported."
	@echo "Target clean:"
	@echo "	Removes all object files."
	@echo "Target test:"
	@echo "	Same as x86, and run in KVM."
	@echo "Target ctest:"
	@echo "	Clean followed by test"
	@echo "Target usage:"
	@echo "	Show this message."
	@echo " "
	@echo "FLAGS="
	@echo "	-D\ MMTEST"
	@echo "		Run the memory test suite when initialising the heap"
	@echo "	-D\ GDTTEST"
	@echo "		Test the GDT. Very simple test suite, just to check if your kernel can continue when the new GDT is installed"
	@echo "	-D\ WARN"
	@echo "		Give warnings instead of panics where possible"
	@echo "	-D\ FAST"
	@echo "		Use some functions which introduce slightly less overhead over the more readable functions"
