#
# Root Makefile
#

MAKE=make

.PHONY: all clean
all:
	@$(MAKE) -C src/ all

clean:
	@$(MAKE) -C src/ clean
