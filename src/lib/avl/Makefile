CC=gcc
CFLAGS=-I../../include -c

DEPS=../../include/lib/tree.h
TARGETS=tree.c

tree: tree.o
	$(CC) tree.o -o tree

tree.o: $(TARGETS) $(DEPS)
	$(CC) $(CFLAGS) $(TARGETS)


.PHONY: clean
clean:
	rm -rfv `find -name *.o`
	rm tree
