[BITS 32]
[EXTERN kmain]
[SECTION .pmode]

pmodemain:
	call kmain
	int 0x19
	jmp $