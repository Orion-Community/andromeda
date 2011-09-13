MAKEEND=FLAGS="$(FLAGS) " DEFS="$(DEFS)" $(DEFS)
MAKE=make

.PHONY: all
all: x86
	mv -fv arch.o ../

.PHONY: x86
x86:
	$(MAKE) $(MAKEEND) -C x86
	mv -fv x86/arch.o ./

.PHONY: clean
clean:
	$(MAKE) -C x86 clean
