[bits 16]
[org 0x1000]

main:
	mov si, exec
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

	exec db 'The second stage bootloader has been called and is executing..', 0x0

times 512 - ($ - $$) db 0