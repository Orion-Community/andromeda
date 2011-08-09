include make/makeIncl
include make/x86

.PHONY: all
all: new

.PHONY: new
new: $(OUTD)

$(OUTD):
	#$(MAKE) -C drivers/ FLAGS="$(FLAGS) "
	$(MAKE) -C kern/ FLAGS="$(FLAGS) "
	$(MAKE) -C mm/ FLAGS="$(FLAGS) "
	$(MAKE) -C nano/ FLAGS="$(FLAGS) "
	$(MAKE) -C math/ FLAGS="$(FLAGS) "
	$(MAKE) -C boot/ FLAGS="$(FLAGS) "
	$(MAKE) -C fs/ FLAGS="$(FLAGS) "
	
	rm -v nano/boot.o nano/kmain.o nano/map.o
	
	#mv -v drivers/drivers.o ./
	mv -v kern/kern.o ./
	mv -v mm/coremm.o ./
	mv -v nano/*.o ./
	mv -v math/maths.o ./
	mv -v boot/*.o ./
	mv -v fs/fs.o ./
	
	mv -v nano/$(OUTC) ./
	
	$(LD) $(LDFLAGS) $(LDCORE) -o $(OUTD) *.o

.PHONY: clean
clean:
	#$(MAKE) -C drivers/ clean
	$(MAKE) -C kern/ clean
	$(MAKE) -C mm/ clean
	$(MAKE) -C nano/ clean
	$(MAKE) -C math/ clean
	$(MAKE) -C fs/ clean
	
	rm -v *.o
	rm -v $(OUTC)
	rm -v $(OUTD)

.PHONY: amd64
amd64:
	@echo "Not yet implemented"

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
