;
;    Load a sector from disk into the memory.
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
loadimage:
	
.checkextensions:
	mov ah, 0x41	; check ext
	mov dl, [bootdisk]	; HDD0
	mov bx, 0x55AA
	int 0x13
	jc .checkextensions

.extread:
	mov ah,0x42
	mov dl, [bootdisk]
	lea si,[lbar]        
	int 0x13
	jnc .return

.oldway:
	xor ah, ah ; function 0 = reset
	mov dl, [bootdisk]
	int 0x13
	jc .oldway

.oldload:
	mov bx, 0x00	; segment
	mov es, bx
	mov bx, 0x7E00	; offset

	mov ah, 0x2					; function 2
	mov al, 0x1					; read 1 sector
	xor ch, ch					; track
	mov cl, 0x2					; sector to read
	xor dh, dh					; head number
	mov dl, [bootdisk]				; drive number
	int 0x13					; call BIOS - Read the sector
	jc .oldload
	
	test ah, ah
	jnz .oldway
	cmp al, 0x1
	jne .oldload

.return:
	ret
;
; LBA register
;
; Loading a sector with this seg:off will place it right after the mbr
;

lbar:
	db 0x10      	; register size
	db 0      	; reserved, must be 0
	dw 0x1      	; sectors to read
	dw 0x7E00   	; memory offset
	dw 0x0   	; memory segment
	dq 0x1		; starting sector (sector to read, s1 = 0)