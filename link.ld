SECTIONS
{
	.bss :
	{
		*(.bss)
	}
	.boot 0x7E00 :
	{
		*(.stage1)
		*(.stage2)

	}

	.end :
	{
		. = ALIGN(2);
		*(.end)
		. = ALIGN(512);
	}
}
