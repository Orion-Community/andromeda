include ../../../scripts/makeIncl
CFLAGS+=-I../../../../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386


.PHONY: all
all: vm.o
	mv -fv vm.o ../

vm.o: vm_init.o vm_user.o vm_alloc.o
	$(LD) -o vm.o *.o $(LDFLAGS)

.PHONY: clean
clean:
	rm -fv *.o
