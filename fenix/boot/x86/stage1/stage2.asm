[bits 16]
[org 0x1000]

main:
;  	xor ax, 0x1000
;  	mov ds, ax
;  	mov es, ax
	
	mov si, go
	call print
	
	cli
	jmp $

;
; Output routines
;

%include '../print.asm'

;
; Data section
;

	go db 'Second stage image has succesfully been read and loaded into memory!', 0x0

times 512 - ($ - $$) db 0