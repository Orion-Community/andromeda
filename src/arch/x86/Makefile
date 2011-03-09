include ../../make/makeIncl
include x86

arch:
	$(CC) -c -I../include interrupts/*.c -nostdlib -fno-builtin -nostdinc -fno-stack-protector -m32
	$(AS) boot/boot.s $(ASFLAGS) -o ../../boot.o

clean:
	rm *.o
