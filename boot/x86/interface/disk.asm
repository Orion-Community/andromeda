;
;    A BIOS disk interface. It is used to communicate with the BIOS and read/write to
;    hard disks. If needed, the LBA input will be converted to CHS.
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

; 
; When called, es contains the buffer segment, di will point to a destination
; buffer offset, ecx contains the amount of sectors to read and ebx is the LBA
; address. DX contains the drive number.
; 
[GLOBAL int13_read]
int13_read:
	push bx
	push cx
	push es
	push di
	push dx
	push bp
	mov bp, sp

.reset:
	xor ah, ah
	mov dx, [bp+2]
	int 0x13
	jc .end

.chs:
	mov ax, 0x800
	mov dx, [bp+2]

	xor di, di	; work around for some buggy bioses
	mov es, di
	int 0x13
	jc .end

	and cl, 00111111b	; max sector number is 0x3f
	inc dh		; make head 1-based
	xor bx, bx	
	mov bl, dh	; store head

	xor dx, dx	; clean modulo storage place
	xor ch, ch	; get all the crap out of ch
	mov ax, word [bp+10]
	div cx		; ax = temp value 	dx = sector (0-based)
	add dx, 1	; make sector 1-based and read second sector
	push dx		; save the sector num for a while

	xor dx, dx	; clean modulo
	div bx		; ax = cylinder		dx = head

	mov ch, al	; low bits of the cylinder
	xor al, al
	shr ax, 2	; [6 ... 7] high bits of the cylinder
	pop bx		; get the sector
	or al, bl	; [0 ... 5] bits of the sector number
	mov cl, al

	shl dx, 8	; move dh, dl
	mov bx, [bp+2]	; bios drive num
	mov dl, bl
	
	mov bx, 0x7e0	; segment 0x7e0
	mov es, bx
	mov bx, 0x200	; buffer offset

	mov al, 1
	mov ah, 0x2
	int 0x13

.end:

	pop bp
	pop dx
	pop di
	pop es
	pop cx
	pop bx

	ret