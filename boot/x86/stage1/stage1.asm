;
;    Golden Eagle Boot loader. The first stage is loaded by the bios at 0x0:0x7C00.
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
[ORG 0x7c00]

jmp short main
nop

main: ; entry point
	jmp 0x0:.flush
.flush:
	cli
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, 0x7c00
	sti

	mov byte [bootdisk], dl

	test byte [si], 0x80
	jz .bailout

	;call loadimage

	jmp .loaded
.bailout:
	mov si, failed
	call println
	xor ax, ax
	int 0x16
	int 0x19
	cli
	jmp $

.loaded:
	mov si, booted
	call println

	mov dl, byte [bootdisk]
	jmp 0x0:0x7E00
	cli
	jmp $

;
; Image loader
;

%include 'boot/x86/stage1/loadimage.asm'

;
; Print routines
;

%include 'boot/x86/println.asm'

;
; Since flat binary is one big heap of code without sections, is the code below some sort of data section.
;
	part_table 	dq 0x0
			dq 0x0
	bootdisk db 0x0
	booted db 'EBL has been loaded by the bios! Executing...', 0x0
	failed db '(0x0) Failed to load the next stage.. ready to reboot. Press any key.', 0x0

times 510 - ($ - $$) db 0
dw 0xAA55