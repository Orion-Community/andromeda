MAKE=make

.PHONY: x86
x86:
	$(MAKE) FLAGS="$(FLAGS)" -C x86
	mv -fv x86/arch.o ./

.PHONY: clean
clean:
	$(MAKE) -C x86 clean
