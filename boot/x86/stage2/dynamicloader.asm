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

dynamicloader:
	
.checkextensions:
	mov ah, 0x41	; check ext
	mov dl, 0x80	; HDD0
	mov bx, 0x55AA
	int 0x13
	jc .checkextensions

.calcsectors:
	xor dx, dx
	lea ax, [endptr]
	sub ax, 0x7E00 ; mbr offset
	sub ax, 0x400 ; stage 1.5 size
	mov bx, 0x200 ; sector size
	idiv bx ; divide size by sector size
	and dx, dx
	jz .extread
	
	inc ax

.extread:
	mov [sectors], ax
	mov ah,0x42
	mov dl,0x80
	lea si,[lbar]        
	int 0x13
	jnc .return

.return:
	ret

lbar:
	db 0x10
	db 0x0
sectors	resw 1    ; ptr to amount of sectors to read
	dw 0x400	; offset
	dw 0x7E0	; segment
	dq 0x2