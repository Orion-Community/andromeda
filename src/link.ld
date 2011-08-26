ENTRY(pmodemain)
phys = 0x100000;

SECTIONS
{	
	.text phys : AT(phys)
	{
		code = .;
		*(.text)
		*(.rodata)
		. = ALIGN(4096);
	}

	.data : AT(phys + (data - code))
	{
		data = .; _data = .; __data = .;
		*(.data)
		. = ALIGN(4096);
	}

	.bss : AT(phys + (bss - code))
	{
		bss = .; _bss = .; __bss = .;
		*(.bss)
		*(COMMON)
                . = ALIGN(4096);
	}
	.end ALIGN(2) : AT(phys + (end - code))
	{
		end = .;
		*(.end)
	}
}
