[BITS 16]
[SECTION .stage2]
[GLOBAL endptr]

stage2main:
	cli
	jmp $


[SECTION .end]
endptr:
	dw 0xBEEF