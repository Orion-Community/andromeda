[BITS 16]
[SECTION .stage2]
[GLOBAL endptr]

stage2main:
	mov si, stage2
	call println
	cli
	jmp $

;
; Print routines
;
%include 'boot/x86/println.asm'

	stage2 db 'Second stage has been loaded.', 0x0
	gdt db 'Implementing a GDT and PMode', 0x0


[SECTION .end]
endptr:
	dw 0xBEEF