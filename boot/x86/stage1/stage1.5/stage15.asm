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

main:
	mov [bootdisk], dl
	
	mov di, 0x7c00
	push di
	mov cx, 0x8
	cld
	rep movsw

	call enable_A20
	jnc .loadstage2
	mov si, a20fail

.bailout:
	call println
	cli
	jmp $

.loadstage2:
	mov si, a20ok
	call println
	
	call getmemorymap
	mov si, a20fail
	jc .bailout

	mov ax, word [mmr+4]
	or ax, ax
	jz .bailout		; we are not here to bully our user with al zero-length memory map.

	call dynamicloader
	shr ax, 8
	or al, al
	mov si, nostage2
	jnz .bailout

	jmp 0x7E0:0x400

	jmp .bailout

;
; Dynamic disk reader
;

%include 'boot/x86/stage1/stage1.5/dynamicloader.asm'

;
; Memory map
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
	
	bootdisk db 0
	a20ok db 'The A20 line has been enabled.', 0x0
	a20fail db '(0x1) The A20 gate couldn`t be opened. Press a key to reboot.', 0x0
	nostage2 db '(0x2) Failed to load the second stage.. Press a key to reboot.', 0x0

times 1024 - ($ - $$) db 0
