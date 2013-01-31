CC=gcc
CFLAGS=-I../../include -Wall -lpthread

DEPS=../../include/lib/tree.h
TARGETS=tree.c

.PHONY: all
all: tree

tree: tree.o
	$(CC) tree.o -o tree $(CFLAGS)

tree.o: $(TARGETS) $(DEPS)
	$(CC) $(CFLAGS) -c $(TARGETS)


.PHONY: clean
clean:
	rm -rfv `find -name *.o`
	rm tree
