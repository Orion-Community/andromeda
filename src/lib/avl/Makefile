include ../../scripts/makeIncl
include ../../scripts/x86

CFLAGS+=-I../../../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

DEPS=../../../include/lib/tree.h
TARGETS=tree.c

.PHONY: avl.0
avl.o: tree.o
	$(LD) $(LDFLAGS) -o avl.o *.o
	mv avl.o ../avl.o

tree.o: $(TARGETS) $(DEPS)
	$(CC) $(CFLAGS) -c $(TARGETS)


.PHONY: clean
clean:
	rm -rfv `find -name *.o`
