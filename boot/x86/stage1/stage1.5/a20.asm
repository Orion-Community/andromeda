;
;    Enable the A20 line with the AT Keyboard controller. It represents the 21st bit (20 when counting from 0) of any mem address.
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

[BITS 16]
[SECTION .stage2]

%include "boot/x86/include/a20.h"

[GLOBAL openA20]
openA20:
	pushfd
	cli
	pusha

	call .writewait
	mov al, OL_DISABLE_KEYBOARD
	out OL_COMMAND_PORT, al

	mov cx, 0x5	; 5 attempts
.atkeyboard1:
	call .writewait	; check if we can write before we write a command
	mov al, OL_READ_OUTPUT_PORT
	out OL_COMMAND_PORT, al	; write command 0xd0 (read output port)
	call .readwait

	xor ax, ax
	in al, OL_OUTPUTBUFFER_PORT	; read output port from the buffer
	or al, 0x2	; enable a20 bit - or al, 00000010b
	push ax		; save the data temp

	call .writewait
	mov al, OL_WRITE_OUTPUT_PORT	; write to output port command (0xd1)
	out OL_COMMAND_PORT, al
	call .writewait

	pop ax
	out OL_DATA_PORT, al	; sent the output register with the enable a20 bit

	call .testA20

	loop .atkeyboard1

	mov cx, 5	; again, 5 tries

.atkeyboard2:
	call .writewait
	mov al, OL_ENABLE_A20_GATE
	out OL_COMMAND_PORT, al	; 0xdf command -> enable a20 gate

	call .testA20

	loop .atkeyboard2

.biosenable:
	sti
	mov ax, 0x2401
	int 0x15
	cli
	jnc .done

%ifndef __OLDPC
.fastA20:
	in al, A20_PORT
	or al, 0x2	; or al, 00000010b - enable a20 bit
	out A20_PORT, al

	call .testA20
%endif
	jmp .failed

;
; Co-routines which help the main routine
;	

.readwait:
	xor ax, ax
	in al, OL_STATUS_PORT	; read status register
	bt ax, 0
	jnc .readwait
	ret

.writewait:
	xor ax, ax
	in al, OL_STATUS_PORT	; read status register
	bt ax, 1
	jc .writewait
	ret

.testA20:
	call .writewait
	mov al, OL_READ_OUTPUT_PORT	; read output port
	out OL_COMMAND_PORT, al
	call .readwait

	xor ax, ax
	in al, OL_OUTPUTBUFFER_PORT	; read output port
	bt ax, 1	; test the second bit
	pop dx	; return address
	jc .done
	jmp dx

.done:
	popa
	popfd
	clc
	ret
.failed:
	popa
	popfd
	stc
	ret