;
;    The masterboot record. Loads the first sector of the active partition.
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

%include "masterboot.h"

[BITS 16]
[ORG 0x7c00]

; This code is located at the first sector of the harddisk. It is loaded by the bios to address 0x0:0x7c00.
; The code will migrate itself to address 0x50:0x0. From there it will load the first sector of the active partition
; to address 0x0:0x7c00.

jmp short _start
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
	db 'FAT12   '                 ; file system type
%else
	bootdisk db 0
%endif

_start:
	cli
; 	we are not safe here
	jmp 0x0:.flush
; 	there are buggy bioses which load you in at 0x0:0x7c00
.flush:
	xor ax, ax
	mov ds, ax 	; tiny memory model

	mov es, ax	; general and extra segmets
	mov gs, ax
	mov fs, ax

	mov ss, ax	; stack segment
	mov sp, LOADOFF
	sti
main:

jmp $

times 510 - ($-$$) db 0
dw 0xaa55
	