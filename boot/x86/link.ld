SECTION
{
	.asm 0x7c00:
	{
		asm = .; _asm = .; __asm = .;
		*(.asm)
	}
	.text:
	{
		code = .; _code = .; __code = .;
		*(.text)
	}
	.final:
	{
		code = .; _code = .; __code = .;
		*(.text)
	}
}