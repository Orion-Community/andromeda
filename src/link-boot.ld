ENTRY(main)
phys = 0x100000;

SECTIONS
{	
	.boot 0x7E00:
	{
		*(.stage1)
		*(.stage2)
		*(.pmode)
		. = ALIGN(4096);
	}
}
