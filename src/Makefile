include make/makeIncl

all: x86

x86:
	$(MAKE) -f Makefile.x86 x86

amd64:
	@echo "Not yet implemented"
clean:
	rm *.o
	rm $(OUTC)

test: all
	kvm -m 512M -kernel compressed
