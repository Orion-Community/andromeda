include make/makeIncl

.PHONY: all
all: x86

.PHONY: x86
x86:
	$(MAKE) -f Makefile.comp x86

.PHONY: amd64
amd64:
	@echo "Not yet implemented"

.PHONY: clean
clean:
	rm *.o
	rm $(OUTC)

.PHONY: all
test: all
	kvm -m 600M -kernel compressed

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
	@echo "	-D\ FASTGDT"
	@echo "		Use a minor optimalisation in the GDT code"