include scripts/makeIncl
include scripts/x86

MAKEEND=FLAGS="$(FLAGS)"
MAKEEND+=FLAGS=" -D X86 -D __INTEL -D __PCI_DEBUG"
#MAKEEND+="-s"

DEPS=start.o arch.o kern.o mm.o text.o maths.o fs.o sys.o panic.o

.PHONY: all
all: x86

.PHONY: amd64
amd64:
	@echo "Not yet implemented"

.PHONY: x86
x86: $(OUT)

$(OUT): $(DEPS)
	$(LD) $(LDFLAGS) $(LDSCRIPT) -o $(OUT) $(DEPS)

# 	$(MAKE) -C error $(MAKEEND)
# 	$(MAKE) -C mm $(MAKEEND)
# 	$(MAKE) -C kern $(MAKEEND)
# 	$(MAKE) -C arch $(MAKEEND)
# 	$(MAKE) -C text $(MAKEEND)
# 	$(MAKE) -C boot $(MAKEEND)
# 	$(MAKE) -C math $(MAKEEND)
# 	$(MAKE) -C fs $(MAKEEND)
# 	$(MAKE) -C sys $(MAKEEND)
# 	$(MAKE) -C drivers $(MAKEEND)

start.o:
	$(MAKE) -C boot $(MAKEEND)
	mv -vf boot/start.o ./

arch.o:
	$(MAKE) -C arch $(MAKEEND)

kern.o:
	$(MAKE) -C kern $(MAKEEND)

mm.o:
	$(MAKE) -C mm $(MAKEEND)

text.o:
	$(MAKE) -C text $(MAKEEND)

maths.o:
	$(MAKE) -C math $(MAKEEND)

fs.o:
	$(MAKE) -C fs $(MAKEEND)

sys.o:
	$(MAKE) -C sys $(MAKEEND)
	mv -vf sys/sys.o ./

panic.o:
	$(MAKE) -C error $(MAKEEND)

driver.o:
	$(MAKE) -C drivers $(MAKEEND)
	
.PHONY: clean
clean:
	rm -fv $(OUT)
	rm -fv `find -name "*.bin"`
	rm -fv `find -name "*.o"`

.PHONY: all
test: all
	qemu -m 600M -kernel $(OUT)
	
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
