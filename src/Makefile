include scripts/makeIncl
include scripts/x86

MAKEEND=FLAGS="$(FLAGS) " DEFS="$(DEFS)" $(DEFS)

.PHONY: all
all: x86

.PHONY: amd64
amd64:
	@echo "Not yet implemented"

.PHONY: x86
x86: $(OUT)

$(OUT):
	$(MAKE) -C error $(MAKEEND)
	$(MAKE) -C mm $(MAKEEND)
	$(MAKE) -C kern $(MAKEEND)
	$(MAKE) -C arch $(MAKEEND)
	$(MAKE) -C text $(MAKEEND)
	$(MAKE) -C boot $(MAKEEND)
	$(MAKE) -C math $(MAKEEND)
	$(MAKE) -C fs $(MAKEEND)
	$(MAKE) -C drivers $(MAKEEND)
	
	$(LD) $(LDFLAGS) $(LDSCRIPT) -o $(OUT) *.o

.PHONY: localClean
localClean: clean
	rm -fv *.o
	rm $(OUTC)
	
.PHONY: clean
clean:
	$(MAKE) -C error clean
	$(MAKE) -C mm clean
	$(MAKE) -C fs clean
	$(MAKE) -C kern clean
	$(MAKE) -C arch clean
	$(MAKE) -C boot clean
	$(MAKE) -C math clean
	rm -f $(OUT)
	rm -f *.o

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
