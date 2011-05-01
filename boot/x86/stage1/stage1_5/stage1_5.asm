[BITS 16]
[EXTERN endptr] ; pointer to the end of stage 2
[SECTION .bss]
lbar:
	db 0x10
	db 0x0
	resw 1  ; ptr to amount of sectors to read
	dw 0x400	; offset
	dw 0x7E0	; segment
	dq 0x3

[SECTION .stage1]

main:
	mov si, a20
	call println

	call enable_A20
	bt ax, 0
	jnc  .loadstage2

.bailout:
	mov si, a20fail
	call println
	xor ah, ah
	int 0x16
	int 0x19
	cli
	jmp $

.loadstage2:
	mov si, a20ok
	call println
	
	call dynamicloader
	shr ax, 8
	or al, al
	jnz .bailout

	jmp 0x7E0:0x400

	cli
	jmp $


;
; Dynamic disk reader
;

%include 'boot/x86/stage2/dynamicloader.asm'

;
; A20 Gate
;

%include 'boot/x86/stage2/enable_A20.asm'

;
; Print routines
;

%include 'boot/x86/println.asm'

	a20 db 'Opening the A20 gate.', 0x0
	a20ok db 'The A20 line has been enabled.', 0x0
	a20fail db 'The A20 gate couldn`t be opened. Press a key to reboot.', 0x0

times 1024 - ($ - $$) db 0