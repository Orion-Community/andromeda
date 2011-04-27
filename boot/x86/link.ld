SECTION
{
	.asm 0x7C00:
	{
		asm = .; _asm = .; __asm = .;
		*(.asm)
		. = ALIGN(512);
	}
	.text:
	{
		code = .; _code = .; __code = .;
		*(.text)
		. = ALIGN(512);
	}
	.final:
	{
		finel = .; final = .; __final = .;
		*(.text)
		. = ALIGN(512);
	}
}