[BITS 16]
[ORG 0x8000]

jmp short main
nop

main: ; entry point
	mov al, 0x42
	mov ah, 0x0E
	xor bh, bh
	int 0x10

jmp $

;
; Print routines
;

; %include 'boot/x86/println.asm'

times 510 - ($-$$) db 0
dw 0xaa55