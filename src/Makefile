include scripts/makeIncl
include scripts/x86

MAKEEND=FLAGS="$(FLAGS) "

MAKEEND+=FLAGS+="-D X86 -D __INTEL -D __IOAPIC_DBG -D __PCI_DEBUG -D MSI -D \
MSIX -D MSIX_DEBUG"
#MAKEEND+=FLAGS+="-D X86 -D __INTEL"

DEPS=arch.o net.o andromeda.o mm.o maths.o sys.o fs.o drivers.o

.PHONY: all
all: x86

.PHONY: amd64
amd64:
	@echo "Not yet implemented"

.PHONY: x86
x86: $(OUT)

$(OUT): $(DEPS)
	$(LD) $(LDFLAGS) $(LDSCRIPT) -o $(OUT) $(DEPS)

# start.o:
# 	$(MAKE) -C boot $(MAKEEND)
# 	mv -vf boot/start.o ./

arch.o:
	$(MAKE) -C arch $(MAKEEND)

andromeda.o:
	$(MAKE) -C andromeda $(MAKEEND)

mm.o:
	$(MAKE) -C mm $(MAKEEND)

maths.o:
	$(MAKE) -C math $(MAKEEND)

sys.o:
	$(MAKE) -C sys $(MAKEEND)
	mv -vf sys/sys.o ./

drivers.o:
	$(MAKE) -C drivers $(MAKEEND)

fs.o:
	$(MAKE) -C fs $(MAKEEND)

net.o:
	$(MAKE) -C net $(MAKEEND)

.PHONY: clean
clean:
	rm -fv $(OUT)
	rm -fv `find -name "*.o"`

.PHONY: test
test: all
	qemu -m 64M -kernel $(OUT) -monitor stdio

.PHONY: ctest
ctest: clean all
	qemu -m 64M -kernel $(OUT) -monitor stdio

.PHONY: install
install: all
	cp -fv ./$(OUT) $(DIR)/boot/andromeda/
	umount $(DIR)

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
	@echo "	Clean followed by test, does not work with the -j option"
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
