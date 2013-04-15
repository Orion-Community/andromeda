include ../../scripts/makeIncl

## -*- Makefile -*-
##
## User: Michel Megens
## Time: 24th of February
## Makefile created by Michel Megens.
##
##

# compliler shizle
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386
CFLAGS+=-I../../../include/ $(FLAGS)

TARGET=list.o
SRC=xorlist.c
OBJS=$(SRC:.c=.o)

.PHONY: all
all: $(TARGET)
	-mv $(TARGET) ../

$(TARGET): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(TARGET)

%o: %c
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	-rm -fv $(OBJS)
