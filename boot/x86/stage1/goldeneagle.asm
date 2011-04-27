;
;    Golden Eagle bootloader. Loads the fenix kernel.
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
[ORG 0x7C00]

section .asm
main:
	mov si, boot
	call print

	call loadimage

	shr ax, 8
	or al, al
	jz .readsector


.fail:
	mov si,failed
	call print
	cli
	jmp $

.readsector:
;	call loadimage
	mov si,loaded
	call print
	jmp 0x7c0:0x200
	cli
	jmp $
;
; Output routines
;

%include 'print.asm'

;
; The image loader
;

;%include 'stage1/devicereset.asm'
;%include 'stage1/biosextensions.asm'
%include 'stage1/imageloader.asm'

;
; Some sort of data section
;

	boot db 'Reading second stage bootloader from disk...', 0x0
	loaded db 'Second stage image has succesfully been read and loaded into memory!', 0x0
	failed db 'Failed to read image... ready to reboot.', 0x0

times 510 - ($ - $$) db 0
dw 0xAA55
%include 'stage2/fenixloader.asm'
