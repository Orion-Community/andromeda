include ../../scripts/makeIncl
CFLAGS+=-I../../../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386


.PHONY: all
all: stdio.o
	mv -fv stdio.o ../

stdio.o: printf.o
	$(LD) -o stdio.o *.o $(LDFLAGS)

printf.o: printf.c
	$(CC) $(FLAGS) $(CFLAGS) printf.c

.PHONY: clean
clean:
	rm -fv *.o
