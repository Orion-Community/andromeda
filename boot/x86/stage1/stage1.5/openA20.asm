openA20:
	cli
	pusha
; If you write data to port 0x64 the cpu inteprents it as a command byte. To sent a data byte sent to port 0x60.
	mov cx, 0x5	; 5 attempts
.atkeyboard1:
	call .writewait	; check if we can write before we write a command
	mov al, 0xd0
	out 0x64, al	; write command 0xd0 (read output port)
	call .readwait

	xor ax, ax
	in al, 0x60	; read output port from the buffer
	or al, 0x2	; enable a20 bit
	push ax		; save the data temp

	call .writewait
	mov al, 0xd1	; output write output port command
	out 0x64, al
	call .writewait

	pop ax
	out 0x60, al	; sent the output register with the enable a20 bit

	call .testA20

	loop .atkeyboard1

	mov cx, 5	; again, 5 tries

.atkeyboard2:
	call .writewait
	mov al, 0xdf
	out 0x64, al	; 0xdf command -> enable a20 gate

	call .testA20

	loop .atkeyboard2
	jmp .failed

;
; Co-routines which help the main routine
;	

.readwait:
	xor ax, ax
	in al, 0x64	; read status register
	bt ax, 0
	jnc .readwait
	ret

.writewait:
	xor ax, ax
	in al, 0x64	; read status register
	bt ax, 1
	jc .writewait
	ret

.testA20:
	call .writewait
	mov al, 0xd0	; read output port
	out 0x64, al
	call .readwait

	xor ax, ax
	in al, 0x60	; read output port
	bt ax, 1	; test the second bit
	pop dx	; return address
	jnc .done
	jmp dx

.done:
	popa
	sti
	clc
	ret
.failed:
	popa
	sti
	stc
	ret