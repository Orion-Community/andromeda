include ../../scripts/makeIncl
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386
CFLAGS+=-I../../../include/

DEPS=timer.c
TARGETS=timer_dev.o

.PHONY: all
all: $(TARGETS)
	mv -vf $(TARGETS) ../timer.o

$(TARGETS): $(DEPS)
	$(CC) $(CFLAGS) $(FLAGS) $(DEPS)
	$(LD) $(LDFLAGS) -o $(TARGETS) *.o

.PHONY: clean
clean:
	rm -fv *.o
