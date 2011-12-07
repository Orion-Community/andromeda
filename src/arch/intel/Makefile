include ../../scripts/makeIncl
MAKEEND=FLAGS="$(FLAGS) " DEFS="$(DEFS)" $(DEFS)
CFLAGS+=-I../../include/

LD=ld
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

## TARGETS
TARGET=intel.o

## DEPS
DEPS=kern.o

.PHONY: all
all: $(TARGET)

$(TARGET): $(DEPS)
	$(LD) $(LDFLAGS) -o $(TARGET) $(DEPS)

kern.o:
	$(MAKE) $(MAKEEND) -C kern
	mv -vf kern/kern.o ./