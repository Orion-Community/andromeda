#
# Root Makefile
#

BUILD=scripts/build andromeda.build -t cores
VERSION:=\"$(shell git describe --tags)\"
ifeq ($(COMPILER_FLAGS),)
CFLAGS:=--cflags "-D VERSION=$(VERSION)"
else
CFLAGS:=--cflags "-D VERSION=$(VERSION) $(COMPILER_FLAGS)"
endif
MAKE=make

.PHONY: all clean preconfigure configure distclean test doxygen
.PHONY: bin/andromeda.img allyes-config allno-config random-config
.PHONY: bin/andromeda.iso test_iso bin
	@$(MAKE) -C src/ clean

all: bin/doxygen.tar.bz2 bin/andromeda.iso 

bin: bin/andromeda.img

bin/andromeda.img: scripts/build
	$(BUILD) $(CFLAGS) $(FLAGS)

clean: scripts/build
	$(BUILD) -c $(FLAGS)

configure: scripts/build
	$(BUILD) --configure

allyes-config: scripts/build
	$(BUILD) --configure --allyes-config

allno-config: scripts/build
	$(BUILD) --configure --allno-config

random-config: scripts/build
	$(BUILD) --configure --random-config

preconfigure: scripts/build
	$(BUILD) --update-depfile

scripts/build:
	scripts/get_build.sh

distclean: clean
	rm -fv scripts/build
	rm -fv .config
	rm -rfv bin/
	rm -rfv doc/doxygen

test: bin/andromeda.img
	scripts/qemu.sh $(DEBUG)

test_iso: bin/andromeda.iso
	scripts/qemu.sh -cdrom $(DEBUG)

doxygen:
	doxygen scripts/Doxyfile

bin/doxygen.tar.bz2: doxygen
	if [ ! -d bin ] ; then mkdir bin; fi
	tar -jcvf bin/doxygen.tar.bz2 doc/doxygen/*

bin/andromeda.iso: bin/andromeda.img
	scripts/create_iso.sh
