include make/makeIncl

all: x86

x86:
	$(MAKE) -f Makefile.x86 x86

amd64:
	@echo "Not yet implemented"

clean:
	rm *.o
	rm $(OUTC)

test: all
	kvm -m 512M -kernel compressed -cpu kvm64

testAMD: all
	kvm -m 512M -kernel compressed -cpu qemu64

ctest: clean test

ctestAMD: clean testAMD

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