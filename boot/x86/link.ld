SECTIONS
{
<<<<<<< HEAD
	.asm 0x7c00 :
=======
	.asm 0x7C00:
>>>>>>> cfa9a1970538c69049bdfdd940013f466329b358
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
<<<<<<< HEAD
		final = .; _final = .; __final = .;
=======
		finel = .; final = .; __final = .;
>>>>>>> cfa9a1970538c69049bdfdd940013f466329b358
		*(.text)
		. = ALIGN(512);
	}
}