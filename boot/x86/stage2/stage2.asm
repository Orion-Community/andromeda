[BITS 16]
[SECTION .bss]
gdt:
	times 8 db 0 ; null descriptor

	GDT_CODE_SEL equ $-gdt ; code selector
		dw 0xFFFF ;limit
		dw 0	  ; base 15:0
		db 0	  ; base 23:16
		db 0x9A   ; type -> ring 0 code and readable
		db 0xCF   ; page granular, 32 bit
		db 0	  ; base 31:24

	GDT_DATA_SEL equ $-gdt ; code selector
		dw 0xFFFF ;limit
		dw 0	  ; base 15:0
		db 0	  ; base 23:16
		db 0x92  ; type -> ring 0 code and writable
		db 0xCF   ; page granular, 32 bit
		db 0	  ; base 31:24
gdt_end: ; to calc size

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

times 1333 db 0

%include 'boot/x86/println.asm'

	stage2 db 'Second stage has been loaded.', 0x0
	pmode db 'Implementing a GDT and PMode', 0x0


[SECTION .end]
endptr:
	dw 0xBEEF