#
# Root Makefile
#

MAKE=make

.PHONY: all clean configure distclean test doxygen
	@$(MAKE) -C src/ clean

all: scripts/build.jar
	java -jar scripts/build.jar andromeda.build -t cores $(FLAGS)

clean: scripts/build.jar
	java -jar scripts/build.jar andromeda.build -t cores -c $(FLAGS)

configure: scripts/build.jar
	java -jar scripts/build.jar --configure

scripts/build.jar:
	scripts/get_build.sh

distclean: clean
	rm -fv scripts/build.jar

test: all
	qemu-system-i386 -kernel bin/andromeda.img -m 64M -monitor stdio

doxygen:
	doxygen scripts/doxyfile
