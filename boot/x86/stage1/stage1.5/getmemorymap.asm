;
;    Get the system memory map from the bios and store it in a global constant.
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
getmemorymap:
	push word 0x50
	pop es
	xor di, di	; destination pointer

	mov [mmr], es
	mov [mmr+2], di

	xor bp, bp ; entry counter
	mov eax, 0xE820
	xor ebx, ebx
	mov ecx, 0x18
	mov edx, 0x534D4150
	mov [es:di+20], dword 1
	int 0x15

	jc .failed
	cmp eax, edx
	jne .failed
	cmp ebx, 0
	je .failed

.success:
	mov [mmr+4], byte 0x5
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