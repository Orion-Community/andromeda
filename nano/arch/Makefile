MAKEEND=FLAGS="$(FLAGS) " DEFS="$(DEFS)" $(DEFS)
MAKE=make

.PHONY: all
all: x86
	mv -fv arch.o ../

x86:
	$(MAKE) $(MAKEEND) -C x86

.PHONY: clean
clean:
	$(MAKE) -C x86 clean
