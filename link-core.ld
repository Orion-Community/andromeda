SECTIONS
{	
	.boot 0x7E00:
	{
		*(.boot)
	}
	.text :
	{
		*(.text)
		. = ALIGN(4096);
	}

	.data :
	{
		data = .; _data = .; __data = .;
		*(.data)
		*(.rodata)
		. = ALIGN(4096);
	}

	.bss :
	{
		bss = .; _bss = .; __bss = .;
		*(.bss)
		. = ALIGN(4096);
	}
	.end ALIGN(2) :
	{
		*(.end)
	}
}
