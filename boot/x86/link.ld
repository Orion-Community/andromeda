SECTIONS
{
	.asm 0x7c00 :
	{
		asm = .; _asm = .; __asm = .;
		*(.asm)
		. = ALIGN(512);
	}
	.text :
	{
		code = .; _code = .; __code = .;
		*(.text)
		. = ALIGN(512);
	}
	.final :
	{
		final = .; _final = .; __final = .;
		*(.text)
		. = ALIGN(512);
	}
}