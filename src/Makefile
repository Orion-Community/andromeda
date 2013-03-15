include scripts/makeIncl
include scripts/x86

MAKEEND=FLAGS="$(FLAGS) "

MAKEEND+=FLAGS+="-D X86 -D __INTEL -D __PCI_DEBUG -D MSI -D WARN -D MSG_DBG"
# MAKEEND+=FLAGS+="-D X86 -D __INTEL"

DEPS=arch.o net.o andromeda.o mm.o maths.o fs.o drivers.o lib.o
ifeq ($(wildcard /etc/fedora-release),)
	QEMU=qemu
else
	QEMU=qemu-system-i386
endif

.PHONY: all
all: x86

.PHONY: stripped
stripped: x86
	strip -s $(OUT)

.PHONY: amd64
amd64:
	@echo "Not yet implemented"

.PHONY: x86
x86: $(DEPS)
	$(MAKE) -C arch x86 $(MAKEEND)
	$(LD) $(LDFLAGS) $(LDSCRIPT) -o $(OUT) *.o

avr: $(DEPS)
	$(MAGE) -C arch avr $(MAKEEND)
	$(LD) $(LDFLAGS) $(LDSCRIPT) -o $(OUT) *.o

# start.o:
# 	$(MAKE) -C boot $(MAKEEND)
# 	mv -vf boot/start.o ./

.PHONY: arch.o andromeda.o mm.o maths.o sys.o drivers.o fs.o net.o
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

lib.o:
	$(MAKE) -C lib $(MAKEEND)

.PHONY: clean
clean:
	rm -fv $(OUT)
	rm -fv `find -name "*.o"`
	rm -fv `find -name *.iso`

.PHONY: test
test: all
	$(QEMU) -kernel $(OUT) -net nic,model=rtl8139,vlan=0,macaddr=48:5d:60:bf:51:a9 \
-net socket,vlan=0,connect=127.0.0.1:8080 -m 64M -monitor stdio

.PHONY: ctest
ctest: clean all
	$(QEMU) -kernel $(OUT) -net nic,model=rtl8139,vlan=1,macaddr=c0:de:00:ba:be:00 -m 64M -monitor stdio

andromeda.iso: all
	scripts/create_iso.sh

.PHONY: iso
iso: andromeda.iso
	$(QEMU) -cdrom andromeda.iso -m 64M -monitor stdio

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
	@echo "Target install:"
	@echo "	Install the kernel to a device mounted at the directory pointed"
	@echo "	to by the DIR variable"
	@echo " "
	@echo "FLAGS="
	@echo "	-D\ MMTEST"
	@echo "		Run the memory test suite when initialising the heap"
	@echo "	-D\ GDTTEST"
	@echo "		Test the GDT. Very simple test suite, just to check if"
	@echo "		your kernel can continue when the new GDT is installed"
	@echo "	-D\ WARN"
	@echo "		Give warnings instead of panics where possible"
	@echo "	-D\ FAST"
	@echo "		Use some functions which introduce slightly less overhead"
	@echo "		over the more readable functions"

.PHONY: sandwich
sandwich:
	@scripts/sandwich.sh

.PHONY: love
love:
	@echo "Not warcraft."

.PHONY: friends
friends:
	@echo "Hi, I'm `hostname`, want to be friends? :3"

.PHONY: time
time:
	@date +%H:%M:%S
