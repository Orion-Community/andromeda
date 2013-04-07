include ../scripts/makeIncl
CFLAGS+=-I../../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

TARGETS=math.o

# complex.o isn't required or something?

.PHONY: all
all: maths.o
	mv -fv maths.o ../

maths.o: $(TARGETS)
	$(LD) $(LDFLAGS) -o $@ *.o

%o: %c
	$(CC) $(CFLAGS) $(FLAGS) $< -o $@

.PHONY: clean
clean:
	rm -fv *.o
