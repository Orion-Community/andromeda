enable_A20:

; Try to enable the A20 line. returns -1 in ax on failure 0 on success.

	pusha
	cli
	mov cx, 5 ; 5 attemps
	call .commandwait

	mov al, 0xD0 ; command 0x0D: read output port
	out 0x64, al
	call .datawait ; sit and wait for it

	xor ax, ax
	in al, 0x60 ; get the data
	push ax ; now save it

	call .commandwait
	mov al, 0xD1
	out 0x64, al

	call .commandwait
	pop ax ; get old value

	; enable A20
	or al, 00000010b
	out 0x60, al
	

.commandwait:
; wait for the controller to be ready
	xor ax, ax
	in al, 0x64
	bt ax, 1
	jc .commandwait
	ret

.datawait:
; wait for data bytes to be ready
	xor ax, ax
	in al, 0x64
	bt ax, 0
	jnc .datawait
	ret