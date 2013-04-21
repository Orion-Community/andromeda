#
# Root Makefile
#

MAKE=make

.PHONY: all clean new_all configure new_clean distclean
all:
	@$(MAKE) -C src/ all

clean:
	@$(MAKE) -C src/ clean

new_all: scripts/build.jar
	java -jar scripts/build.jar andromeda.build -vt cores $(FLAGS)

new_clean: scripts/build.jar
	java -jar scripts/build.jar andromeda.build -vt cores -c $(FLAGS)

configure: scripts/build.jar
	java -jar scripts/build.jar --configure

scripts/build.jar:
	scripts/get_build.sh

distclean: new_clean
	rm -fv scripts/build.jar
