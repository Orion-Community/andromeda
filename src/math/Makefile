include ../scripts/makeIncl
CFLAGS+=-I../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

TARGETS=maths.o 

.PHONY: all
all: $(TARGETS)
	mv -fv maths.o ../

$(TARGETS): math.o
	$(LD) $(LDFLAGS) -o $(TARGETS) *.o

math.o:
	$(CC) $(CFLAGS) $(FLAGS) math.c

complex.o:
	$(CC) $(CFLAGS) $(FLAGS) complex.c

.PHONY: clean
clean:
	rm -fv *.o
