SECTIONS
{
	.boot 0x7E00 :
	{
		. = ALIGN(512);
		*(.stage1)
		*(.stage2)
	}
	.end :
	{
		. = ALIGN(2);
		*(.end)
	}
}
