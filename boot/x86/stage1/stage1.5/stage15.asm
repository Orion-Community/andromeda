;
;    Entry point for stage 1.5. This sector will use a dynamic sector loader to load the second stage and the micro kernel.
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
[SECTION .stage1]

jmp short main
nop

main:	
	mov di, 0x7c00
	push di
	mov cx, 0x8
	cld
	rep movsw
	push dx

	jmp .loadstage2

.bailout:
	mov si, failed
	call println
	cli
	jmp $

.loadstage2:
	call enable_A20
	jc .bailout

	call getmemorymap
	jc .bailout
%ifdef __DEBUG
	push word 0x50
	pop es
	cmp [es:0x28], dword 0x2	; this mem entry should be reserved memory
	jne .bailout
%endif

	call dynamicloader
	jc .bailout

	pop dx
	pop si
	jmp 0x7E0:0x400

	jmp .bailout

;
; Dynamic disk reader
;

%include 'boot/x86/stage1/stage1.5/dynamicloader.asm'

;
; Memory map
;
; 
%include 'boot/x86/stage1/stage1.5/getmemorymap.asm'

;
; A20 Gate
;

%include 'boot/x86/stage1/stage1.5/enable_A20.asm'

;
; Print routines
;

%include 'boot/x86/println.asm'
	
	failed db '0x2', 0x0

times 1024 - ($ - $$) db 0
