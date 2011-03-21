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
	kvm -m 512M -kernel compressed

ctest: clean test

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
	@echo "This file supports the following FLAGS"
	@echo "	FLAGS=-D\\ TESTALLOC"
	@echo "		Enables memory allocation testing"
	@echo "	FLAGS=-D\\ TESTALLIGNED"
	@echo "		Enables page alligned memory allocation testing"