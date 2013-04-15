include ../../scripts/makeIncl
CFLAGS+=-I../../../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../../../include/" $(FLAGS)

# targets

KBD=kbd.o
TARGETS=ps2.o kbd.o

.PHONY: all
all: kdb

kdb: $(TARGETS)
	$(LD) $(LDFLAGS) $(TARGETS) -o ../$(KBD)

%o: %c
	$(CC) $(CFLAGS) $(FLAGS) $< -o $@

.PHONY: clean
clean:
	rm -vf *.o
