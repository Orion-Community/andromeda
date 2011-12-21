include ../../scripts/makeIncl
CFLAGS+=-I../../include/

TARGETS=out.c
DEPS=include/out.h

.PHONY: all
all: text.o
	mv -fv text.o ../

text.o: $(TARGETS) $(DEPS)
	$(CC) $(CFLAGS) $(FLAGS) $(TARGETS) -o text.o
