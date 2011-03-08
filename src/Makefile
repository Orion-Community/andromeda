all:
	gcc -c */*.c -Iinclude -nostdlib -fno-builtin -nostdinc -fno-stack-protector -m32
	nasm boot/boot.s -felf
	ld -Tlink.ld -oformat=elf32-i386 -melf_i386 -o kernel *.o boot/*.o
clean:
	rm *.o
	rm boot/*.o
	rm kernel
