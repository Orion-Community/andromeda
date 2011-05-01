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
		*(.pmode)
	}
	.text :
	{
		*(.text)
	}
	.data :
	{
		*(.data)
	}
	.end ALIGN(2) :
	{
		*(.end)
	}
}
