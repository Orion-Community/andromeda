include ../scripts/makeIncl
CFLAGS+=-I../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

DEPS=math.c
TARGETS=maths.o
HEADERS=

.PHONY: all
all: $(TARGETS)
	mv -fv maths.o ../

$(TARGETS): $(DEPS) $(HEADERS)
	$(CC) $(CFLAGS) $(FLAGS) $(DEPS)
	$(LD) $(LDFLAGS) -o $(TARGETS) *.o

.PHONY: clean
clean:
	rm -fv *.o
