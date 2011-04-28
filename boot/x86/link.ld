SECTIONS
{
	.boot 0x7c00 :
	{
		*(.boot)
	}
	.text :
	{
		*(.text)
	}
	.final :
	{
		*(.final)
	}
}