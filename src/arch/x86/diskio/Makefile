AS=nasm
ASFLAGS=-felf32 -I"../../../include/" $(FLAGS)

# targets
IDE=ide.o

#sources
IDE_SRC=ide.asm


.PHONY: all
all: $(IDE)

$(IDE): $(IDE_SRC)
	$(AS) $(ASFLAGS) $(IDE_SRC) -o $(IDE)

.PHONY: clean
clean:
	rm -v *.o
