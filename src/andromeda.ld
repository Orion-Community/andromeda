OUTPUT_FORMAT(elf32-i386)
ENTRY(start)
SECTIONS
{
    .boot 0x100000 :
    {
	boot = .; _boot = .; __boot = .;
	*(.boot);
	. = ALIGN(0x1000);
    }

    . += 0xC0000000;

    .higherhalf : AT (ADDR(.higherhalf) - 0xC0000000)
    {
        higherhalf = .; _higherhalf = .; __higherhalf = .;
        *(.higherhalf);
        . = ALIGN(0x1000);
    }
    .text : AT (ADDR(.text) - 0xC0000000)
    {
        code = .; _code = .; __code = .;
        *(.text)
        *(.rodata)
        . = ALIGN(0x1000);
    }
    .data : AT (ADDR(.data) - 0xC0000000)
    {
        data = .; _data = .; __data = .;
        *(.data)
        . = ALIGN(0x1000);
	stack = ALIGN(0x1000);
    }
    .bss : AT (ADDR(.bss) - 0xC0000000)
    {
        bss = .; _bss = .; __bss = .;
        *(.bss)
	*(COMMON)
        . = ALIGN(0x1000);
    }
    end = .; _end = .; __end = .;
}
