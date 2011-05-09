[BITS 16]
[ORG 0x1000]
main: ; entry point
	mov si, booted
	call println

	cli
	jmp $

;
; Print routines
;

%include 'boot/x86/println.asm'

;
; Since flat binary is one big heap of code without sections, is the code below some sort of data section.
;

	booted db 'Got it!', 0x0

times 510 - ($ - $$) db 0
dw 0xAA55