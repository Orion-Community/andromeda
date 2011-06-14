SECTIONS
{	
	.boot 0x7E00:
	{
		*(.stage1)
		*(.stage2)
		*(.pmode)
	}
	.text :
	{
		*(.text)
		*(.rodata)
		*(.data)
		*(.bss)
	}
	.end ALIGN(2) :
	{
		*(.end)
	}
}
