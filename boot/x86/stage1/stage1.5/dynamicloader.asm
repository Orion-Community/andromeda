;
;    Calculate how many sectors the second stage is, and load enough sectors. Algorith used:
;	sectors to read = (endptr - (stage15Offset + stage15Size)) / sectorSize
;
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

[GLOBAL sectorcount]
[EXTERN endptr] ; pointer to the end of stage 2

dynamicloader:
%ifdef __HDD
	pop word [returnaddr]
	pop dx
	pop si
	push si
	push dx

.reset:
	xor ah, ah ; function 0 = reset
	pop dx
	push dx
	int 0x13
	jc .return

.checkextensions:
	mov ax, 0x4100	; check ext
	pop dx
	push dx
	mov bx, 0x55AA
	int 0x13
	jc .chs

.extread:
	call .calcsectors
	mov [dap+2], ax

	pop dx
	push dx			; restore drive num
	mov ax, word [si+8]	; low word of lba
	mov cx, word [si+10]	; high word of lba
	add ax, 3
	push si			; temp save pt
	mov si, dap
	mov [si+8], ax
	mov [si+10], cx
	mov ax, 0x4200
	int 0x13
	pop si
	jnc .return

; If int 0x13 extensions are not supported, use CHS to read the sectors..

.chs:
	mov ax, 0x800
	pop dx
	push dx
	xor di, di	; work around for some buggy bioses
	mov es, di
	int 0x13
	jc .return

	and cl, 00111111b	; max sector number is 0x3f
	inc dh		; make head 1-based
	xor bx, bx	
	mov bl, dh	; store head

	xor dx, dx	; clean modulo storage place
	xor ch, ch	; get all the crap out of ch
	mov ax, word [si+8]	; the pt
	div cx		; ax = temp value 	dx = sector (0-based)
	add dx, 4	; make sector 1-based and read second sector
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
	pop bx		; pop bios drive num off
	mov dl, bl	; push bios drive num
	push bx
	
	mov bx, 0x7e0	; segment 0
	mov es, bx
	mov bx, 0x400	; buffer
	
	call .calcsectors
	mov ah, 0x2
	int 0x13

.return:
	push word [returnaddr]
	ret

.calcsectors:
	lea ax, [endptr] ; adress of the end
	sub ax, 0x8200 ; offset of stage 1.5 (0x7E00) + its file size (0x400) = size
	test ax, 0x1FF ; ax % 512
	jz .powof2
	
	shr ax, 9 ; ax / 512 = amount of sectors
	inc ax
	mov [sectorcount], ax
	ret
.powof2:
	shr ax, 9 
	mov [sectorcount], ax
	ret

dap:
	db 0x10
	db 0x0
	dw 0x0		; amount of sectors to read
	dw 0x400	; offset
	dw 0x7E0	; segment
	dq 0x0		; sector to start at

returnaddr dw 0
sectorcount dw 0
%endif