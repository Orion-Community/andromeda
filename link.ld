SECTIONS
{
	.boot 0x7E00 :
	{
		*(.stage1)
		*(.stage2)
	}

	.end :
	{
		*(.end)
	}
}
