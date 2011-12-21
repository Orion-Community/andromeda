include ../../scripts/makeIncl
CFLAGS+=-I../../include/
LDFLAGS=-r -nostdlib --oformat=elf32-i386 -melf_i386

# compliler shizle
AS=nasm
ASFLAGS=-felf32 -I"../../include/" $(FLAGS)

# targets

KBD=kbd.o
KBD_D=ps2.o kbd_.o

#sources

PS2=ps2.c
KBD_=kbd.c

.PHONY: all
all: $(KBD)

$(KBD): $(KBD_D)
	$(LD) $(LDFLAGS) $(KBD_D) -o $(KBD)
	mv -vf ./$(KBD) ../$(KBD)

ps2.o: $(PS2)
	$(CC) $(CFLAGS) $(FLAGS) $(PS2) -o ps2.o

kbd_.o: $(KBD_)
	$(CC) $(CFLAGS) $(FLAGS) $(KBD_) -o kbd_.o

.PHONY: clean
clean:
	rm -vf *.o
