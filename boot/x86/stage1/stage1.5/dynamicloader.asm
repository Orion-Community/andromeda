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

.checkextensions:
	mov ah, 0x41	; check ext
	mov dl, [bootdisk]	; HDD0
	mov bx, 0x55AA
	int 0x13
	jc .checkextensions

.extread:
 	call .calcsectors
 	mov [lbar+2], ax
	mov ah,0x42

	pop bx	; address pushed on the stack by 'call mem16'
	pop si	; address to the partition table

	mov cx, word [si+8]
	add cx, 0x3
	push si
	push bx

	mov [lbar+8],cx
	lea si, [lbar]

	mov dl,[bootdisk]
	lea si,[lbar]        
	int 0x13
	jnc .return

; .oldreset:
; 	xor ah, ah ; function 0 = reset
; 	mov dl, byte [bootdisk]
; 	int 0x13
; 	jc .oldreset
; 
; .chs:
; 	mov ax, 0x800
; 	mov dl, byte [bootdisk]
; 	xor di, di	; work around for some buggy bioses
; 	mov es, di
; 	int 0x13
; 	jc .return
; 
; 	and cl, 00111111b	; max sector number is 0x3f
; 	inc dh		; make head 1-based
; 	xor bx, bx
; 	mov bl, dh	; store head
; 
; 	xor dx, dx	; clean modulo storage place
; 	xor ch, ch	; get all the crap out of ch
; 	mov ax, word [si+8]	; the pt
; 	div cx		; ax = temp value 	dx = sector (0-based)
; 	add dx, 5	; make sector 1-based and read second sector
; 	push dx		; save the sector num for a while
; 
; 	xor dx, dx	; clean modulo
; 	div bx		; ax = cylinder		dx = head
; 
; 	mov ch, al	; low bits of the cylinder
; 	xor al, al
; 	shr ax, 2	; shift the 2 high bits of the cylinder into al
; 	pop bx		; get the sector
; 	or al, bl	; store sector in al together with high cyl
; 	mov cl, al
; 
; 	shl dx, 8	; move dh, dl
; 	mov dl, byte [bootdisk]
; 	
; 	xor bx, bx	; segment 0
; 	mov es, bx
; 	mov bx, 0x7e00	; buffer
; 	
; 	call .calcsectors
; 	mov ah, 0x2
; 	int 0x13

.return:
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

lbar:
	db 0x10
	db 0x0
	dw 0x0		; amount of sectors to read
	dw 0x400	; offset
	dw 0x7E0	; segment
	dq 0x0		; start to read at sector 4

sectorcount dw 1