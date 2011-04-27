;
;    Read an executable image from the floppy drive or the hard disk.
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
	mov cx, 5

.extreset:
	mov ah, 0x41
	mov dl, 0x80
	mov bx, 0x55AA
	int 0x13
	jc .extreset

.calcsector:
	xor dx, dx  ; The modulo

	lea ax, [last] ; pointer to the last word
	sub ax, 0x7C00 ; offset
	sub ax, 510    ; own size
	mov bx, 512  ; sector size
	idiv bx
	and dx, dx
	jz .extload

	inc ax

.extload:
	mov [sector], ax

	mov ah,0x42
	mov dl,0x80
	lea si,[lbaadr]        
	int 0x13
	jnc .return

	loop .extload

; .oldway:
; 	mov cx, 5
; 
; .oldreset:
; 	xor ah, ah ; function 0 = reset
; 	mov dl, 0x80
; 	int 0x13
; 	jnc .oldload
; 
; 	loop .oldreset
; 
; .oldload:
; 	mov bx, 0x7C0	; segment
; 	mov es, bx
; 	mov bx, 0x200	; offset
; 
; 	mov ah, 0x2					; function 2
; 	mov al, 0x1					; read 1 sector
; 	xor ch, ch					; track
; 	mov cl, 0x2					; sector to read
; 	xor dh, dh					; head number
; 	mov dl, 0x80					; drive number
; 	int 0x13					; call BIOS - Read the sector
; 	
.return:
	ret

;
; Logical Block Addressing adress
;

lbaadr:
	db 10h      	; packet size (16 bytes)
	db 0      	; reserved, must be 0
sector  dw 1		; sectors to read
	dw 0x200   	; Buffer's offset
	dw 0x7C0   	; Buffer's segment
	dq 0x1		; starting sector (sector to read)
