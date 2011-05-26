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
	mov cx, 5

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
	mov dl,[bootdisk]
	lea si,[lbar]        
	int 0x13
	jnc .return

	loop .extread

.oldreset:
	xor ah, ah ; function 0 = reset
	mov dl, [bootdisk]
	int 0x13
	jc .oldreset

.oldload:
	mov  bx, 0x7E0 ; segment
	mov es, bx
	mov bx, 0x400  ; offset

	call .calcsectors
	mov ah, 0x02 ; func 2
	; mov al, sectorcount -> done by calcsectors
	xor ch, ch ; track
	mov cl, 0x4 ; sector to start
	xor dh, dh ; head
	mov dl, [bootdisk] ; drive
	int 0x13

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
	dw 0  ; ptr to amount of sectors to read
	dw 0x400	; offset
	dw 0x7E0	; segment
	dq 0x3		; start to read at sector 4

sectorcount dw 1