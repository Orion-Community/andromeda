include ../../../scripts/makeIncl
CFLAGS+=-I../../../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386


.PHONY: all
all: pte.o
	mv -fv pte.o ../

pte.o: pte_init.o pte_user.o
	$(LD) -o pte.o *.o $(LDFLAGS)

pte_init.o: pte_init.c
	$(CC) $(FLAGS) $(CFLAGS) pte_init.c

pte_user.o: pte_user.c
	$(CC) $(FLAGS) $(CFLAGS) pte_user.c

.PHONY: clean
clean:
	rm -fv *.o
