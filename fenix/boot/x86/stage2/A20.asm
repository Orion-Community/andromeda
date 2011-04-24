;
;    Enable the A20 line. It represents the 21st bit (20 when counting from 0) of any mem address.
;    Copyright (C) 2011 Michel Megens
;
;    This program is free software: you can redistribute it and/or modify
;    it under the terms of the GNU General Public License as published by
;    the Free Software Foundation, either version 3 of the License, or
;    (at your option) any later version.
;
;    This program is distributed in the hope that it will be useful,
;    but WITHOUT ANY WARRANTY; without even the implied warranty of
;    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;    GNU General Public License for more details.
;
;    You should have received a copy of the GNU General Public License
;    along with this program.  If not, see <http://www.gnu.org/licenses/>.
;

enable_A20:

; Try to enable the A20 line. returns -1 in ax on failure 0 on success.
	cli
	pusha
	mov cx, 5 ; 5 attemps

.startAttempt:
	call .commandwait

	mov al, 0xD0 ; command 0xD0: read output port
	out 0x64, al
	call .datawait ; sit and wait for it

	xor ax, ax
	in al, 0x60 ; get the data (status)
	push ax ; now save it

	call .commandwait
	mov al, 0xD1 ; write status bit
	out 0x64, al

	call .commandwait
	pop ax ; get old value

	; enable A20
	or al, 00000010b
	out 0x60, al

	; Now we are going to test the A20 line
	call .commandwait
	mov al, 0xD0 ; read output port
	out 0x64, al
	call .datawait

	xor ax, ax
	in al, 0x60
	bt ax, 00000010b ; bit test ax -> copy bit 1 in CF
	jc .A20_enabled

	; Loop for max 5 times
	loop .startAttempt

	mov cx, 5

.startAttempt2:
	; On some machine's is this the only way to enable the A20 line
	call .commandwait
	mov al, 0xDF
	out 0x64, al

	; Test the A20 line
	call .commandwait
	mov al, 0xD0
	out 0x64, al
	call .datawait

	xor ax, ax
	in al, 0x60
	bt ax, 00000010b
	jc .A20_enabled

	; failed, try again
	loop .startAttempt2

	; failed
	jmp .failed
	

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

.A20_enabled:
	popa
	xor ax, ax
	sti
	ret

.failed:
	popa
	mov ax, 1
	sti
	ret
