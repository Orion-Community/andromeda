;
;    Use the 0x13 extensions to read a sector from disk.
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

;
; Reset the drive
;
loadimage:
	mov cx, 5

.reset:
	mov ah, 0x41
	mov dl, 0x80
	mov bx, 0x55AA
	int 0x13
	jc .reset

.read:
	mov ah,0x42
	mov dl,0x80
	lea si,[lbaadr]        
	int 0x13
	
.return:
	ret

;
; Disk address
;
lbaadr:
	db 10h      ; packet size (16 bytes)
	db 0      ; reserved, must be 0
	dw 0x1      ; number of sectors to transfer
	dw 0x000   ; Buffer's offset
	dw 0x100   ; Buffer's segment
	dq 0x1 ; 64-bit starting sector number
