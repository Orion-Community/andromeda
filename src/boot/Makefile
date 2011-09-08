# compiler sizzle
MAKE=make

# disk images
MASTERBOOT=masterboot.bin
STAGE1=stage1.bin

# linker meat
BOOT=boot.o

.PHONY: all
all: x86

.PHONY: x86
x86:
	$(MAKE) FLAGS="$(FLAGS)" -C x86
	mv -v x86/$(MASTERBOOT) ./
	mv -v x86/$(STAGE1) ./
	mv -v x86/$(BOOT) ./

.PHONY: clean
clean:
	$(MAKE) -C x86 clean
