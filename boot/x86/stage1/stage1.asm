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
; The bios parameter block, in the case it is a floppy
%ifdef __FLOPPY
	times 8 db 0			; fill up 
	dw 512				; bytes per sector 
	db 1 				; sectors per cluster 
	dw 2 				; reserved sector count 
	db 2 				;number of FATs 
	dw 16*14 			; root directory entries 
	dw 18*2*80 			; 18 sector/track * 2 heads * 80 (density) 
	db 0F0h 			; media byte 
	dw 9 				; sectors per fat 
	dw 18 				; sectors per track 
	dw 2 				; number of heads 
	dd 0 				; hidden sector count 
	dd 0 				; number of sectors huge 
	bootdisk db 0 			; drive number 
	db 0 				; reserved 
	db 29h 				; signature 
	dd 0 				; volume ID 
	times 11 db 0	 		; volume label 
	db 'FAT12   '			; file system type
%else
	bootdisk db 0
%endif

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
	call loadimage
	shr ax, 8
	or al, al
	jz .loaded

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
	mov si, failed
	call println
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

	booted db 'GEBL has been loaded by the bios! Executing...', 0x0
	failed db '(0x0) Failed to load the next stage.. ready to reboot. Press any key.', 0x0

times 510 - ($ - $$) db 0
dw 0xAA55