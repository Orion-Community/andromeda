OUTPUT_FORMAT(elf32-i386)
ENTRY(start)
SECTIONS
{
    .boot 0xC0000000 :
    {
      boot = .; _boot = .; __boot = .;
      *(.boot)
      . = ALIGN (0x1000);
    }
    .text :
    {
        code = .; _code = .; __code = .;
        *(.text)
        . = ALIGN(0x1000);
    }
    .data :
    {
        data = .; _data = .; __data = .;
        *(.data)
        *(.rodata)
        . = ALIGN(0x1000);
    }
    .bss :
    {
        bss = .; _bss = .; __bss = .;
        *(.bss)
        . = ALIGN(0x1000);
    }
    end = .; _end = .; __end = .;
}
