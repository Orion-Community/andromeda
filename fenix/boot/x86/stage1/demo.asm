[BITS 16]
[ORG 0x7C00]

main:
;	call loadimage
	call resetdrive
	mov al, 'y'
	mov ah, 0x0E
	xor bh, bh	; page 0
	int 0x10

	jmp $

;
; Sector load routines
;

%include 'biosextensions.asm'
%include 'devicereset.asm'

times 510 - ($-$$) db 0
dw 0xAA55