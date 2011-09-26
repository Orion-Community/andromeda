OUTPUT_FORMAT(elf32-i386)
ENTRY(start)
SECTIONS
{
    .boot 0x100000 :
    {
	boot = .; _boot = .; __boot = .;
	*(.boot)
	. = ALIGN(0x1000);
    }
    .text :
    {
        code = .; _code = .; __code = .;
        *(.text)
        *(.rodata)
        . = ALIGN(0x1000);
    }
    .data :
    {
        data = .; _data = .; __data = .;
        *(.data)
        . = ALIGN(0x1000);
	stack = ALIGN(0x1000);
    }
    .bss :
    {
        bss = .; _bss = .; __bss = .;
        *(.bss)
	*(COMMON)
        . = ALIGN(0x1000);
    }
    end = .; _end = .; __end = .;
}
