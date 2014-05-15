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
    .PD : AT (ADDR(.PD))
    {
        PD = .; _PD = .; __PD = .;
        *(.PD)
        . = ALIGN(0x1000);
    }
    page_dir_boot = .; _page_dir_boot = .; __page_dir_boot = .;
    . += 0x1000;
    page_table_boot = .; _page_table_boot = .; __page_table_boot = .;
    . += 0xC0100000;
    initial_slab_space = .;
    . += 0xA8000;

    .higherhalf : AT (ADDR(.higherhalf) - 0xC0000000)
    {
        higherhalf = .; _higherhalf = .; __higherhalf = .;
        *(.higherhalf);
        . = ALIGN(0x10000);
    }
    .text : AT (ADDR(.text) - 0xC0000000)
    {
        code = .; _code = .; __code = .;
        *(.text)
	*(.gnu_linkonce.t*)
        . = ALIGN(0x10000);
    }
    .rodata : AT (ADDR(.rodata) - 0xC0000000)
    {
	  rodata = .; _rodata = .; __rodata = .;
	  _ctor_start = .;
	  *(.ctor*)
	  _dtor_start = .;
	  *(.dtor*)
	  *(.gnu_linkonce.r*)
	  *(.rodata*)
	  . = ALIGN(0x10000);
    }
    .data : AT (ADDR(.data) - 0xC0000000)
    {
        data = .; _data = .; __data = .;
        *(.data)
	*(.gnu.linkonce.d*)
        . = ALIGN(0x10000);
	stack = ALIGN(0x10000);
    }
    .bss : AT (ADDR(.bss) - 0xC0000000)
    {
        bss = .; _bss = .; __bss = .;
	*(.gnu.linkonce.b*)
        *(.bss)
	*(COMMON)
        . = ALIGN(0x10000);
    }
    end = .; _end = .; __end = .;
}
