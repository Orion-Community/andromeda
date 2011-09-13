include ../../scripts/makeIncl
MAKEEND=FLAGS="$(FLAGS) " DEFS="$(DEFS)" $(DEFS)
CFLAGS+=-I../../include/

LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

KERNEL=kernel.o
MM=mm.o

.PHONY: all
all: arch.o


arch.o: $(KERNEL) $(MM)
	$(CC) $(CFLAGS) $(FLAGS) */*.c
	$(MAKE) -C asm $(MAKEEND)
	$(LD) -o arch.o *.o $(LDFLAGS)

#$(MM):
#	$(MAKE) $(FLAGS) -C mm/

$(KERNEL):
	$(MAKE) $(MAKEEND) -C kernel/
	mv kernel/$(KERNEL) ./

.PHONY: clean
clean:
	$(MAKE) -C kernel clean
	$(MAKE) -C asm clean
	rm -fv *.o
#	$(MAKE) -C mm clean
