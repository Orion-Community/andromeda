#
# Root Makefile
#

BUILD=java -jar scripts/build.jar andromeda.build -t cores
VERSION:=\"$(shell git describe --tags)\"
ifeq ($(COMPILER_FLAGS),)
CFLAGS:=--cflags "-D VERSION=$(VERSION)"
else
CFLAGS:=--cflags "-D VERSION=$(VERSION) $(COMPILER_FLAGS)"
endif
MAKE=make

.PHONY: all clean configure distclean test doxygen bin/andromeda.img 
.PHONY: bin/andromeda.iso test_iso
	@$(MAKE) -C src/ clean

all: bin/andromeda.img

bin/andromeda.img: scripts/build.jar
	$(BUILD) $(CFLAGS) $(FLAGS)

clean: scripts/build.jar
	$(BUILD) -c $(FLAGS)

configure: scripts/build.jar
	$(BUILD) --configure

scripts/build.jar:
	scripts/get_build.sh

distclean: clean
	rm -fv scripts/build.jar
	rm -fv bin/andromeda.iso

test: all
	scripts/qemu.sh

test_iso: bin/andromeda.iso
	scripts/qemu_iso.sh

doxygen:
	doxygen scripts/Doxyfile

bin/andromeda.iso: bin/andromeda.img
	scripts/create_iso.sh
