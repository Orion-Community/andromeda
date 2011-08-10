ENTRY(pmodemain)
phys = 0x100000;

SECTIONS
{	
	.text phys : AT(phys)
	{
		code = .;
		*(.text)
		. = ALIGN(4096);
	}

	.data : AT(phys + (data - code))
	{
		data = .; _data = .; __data = .;
		*(.data)
		*(.rodata)
		. = ALIGN(4096);
	}

	.bss : AT(phys + (bss - code))
	{
		bss = .; _bss = .; __bss = .;
		*(.bss)
		. = ALIGN(4096);
	}
	.end ALIGN(2) : AT(phys + (end - code))
	{
		end = .;
		*(.end)
	}
}
