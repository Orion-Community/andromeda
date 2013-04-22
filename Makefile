#
# Root Makefile
#

MAKE=make

.PHONY: all clean new_all configure new_clean distclean test
all:
	@$(MAKE) -C src/ all

clean:
	@$(MAKE) -C src/ clean

new_all: scripts/build.jar
	java -jar scripts/build.jar andromeda.build -t cores $(FLAGS)

new_clean: scripts/build.jar
	java -jar scripts/build.jar andromeda.build -t cores -c $(FLAGS)

configure: scripts/build.jar
	java -jar scripts/build.jar --configure

scripts/build.jar:
	scripts/get_build.sh

distclean: new_clean
	rm -fv scripts/build.jar

test: new_all
	qemu-system-i386 -kernel bin/andromeda.img -m 64M
