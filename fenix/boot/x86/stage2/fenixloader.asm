[bits 16]
[org 0x1000]

main:
	mov si, exec
	call print
	
	call enable_A20
	xor ax, 0001b
	jnz .a20success

.a20fail:
	mov si, a20failed
	call print
	jmp .bailout

.a20success:
	mov si, a20ok
	call print

.bailout:
	mov si, reboot
	call print
	xor ah, ah
	int 0x16 ; wait for char
	int 0x19 ; this bios loads sector 1 into 0x0:0x7C00 and executes (= reboot)
	
	cli
	jmp $

;
; Output routines
;

%include 'print.asm'

;
; A20 line
;

%include 'stage2/A20.asm'

;
; Data section
;

	exec db 'The second stage bootloader has been called and is executing..', 0x0
	a20ok db 'A20 Line is successfully enabled!', 0x0
	a20failed db 'Setting up the A20 line failed, ready to reboot...', 0x0
	reboot db 'Press a key to reboot', 0x0

times 512 - ($ - $$) db 0
