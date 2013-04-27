#
# Root Makefile
#

BUILD=java -jar scripts/build.jar andromeda.build -t cores
VERSION:=\"$(shell git describe)\"
ifeq ($(COMPILER_FLAGS),)
CFLAGS:=--cflags "-D VERSION=$(VERSION)"
else
CFLAGS:=--cflags "-D VERSION=$(VERSION) $(COMPILER_FLAGS)"
endif
MAKE=make

.PHONY: all clean configure distclean test doxygen
	@$(MAKE) -C src/ clean

all: scripts/build.jar
	$(BUILD) $(CFLAGS) $(FLAGS)

clean: scripts/build.jar
	$(BUILD) -c $(FLAGS)

configure: scripts/build.jar
	java -jar scripts/build.jar --configure

scripts/build.jar:
	scripts/get_build.sh

distclean: clean
	rm -fv scripts/build.jar

test: all
	qemu-system-i386 -kernel bin/andromeda.img -m 64M -monitor stdio

doxygen:
	doxygen scripts/Doxyfile
