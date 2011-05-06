;
;    THIS IS PURE FOR TESTNG PURPOSES.
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
pointertest:
	push word 0x50
	pop es
	xor di, di

	mov [mmr], es
	mov [mmr+2], di

	mov [es:di], dword 0x4587458621365884
	add di, 8
	mov [es:di], dword 0x123

	mov ax, word [mmr]
	mov es, ax
	mov di, [mmr+2]

	cmp [es:di], dword 0x4587458621365884
	jne .failed

	cmp [es:di+8], dword 0x123
	jne .failed
	
.success:
	clc	; clear carry flag
	ret
.failed:
	stc 	; set the carry flag
	ret


mmr:
	dw 0 ; segment
	dw 0 ; offset
	db 0 ;entry count
	db 24 ; entry size