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
	xor bp, bp ; counter
	xor ebx, ebx ; must be 0
	xor eax, eax

	push word 0x50
	pop es
	mov di, 0x0
	
	mov [mmr], es	; store segment
	mov [mmr+2], di ; store offset
	
	mov eax, 0xE820 ; interrupt function number
	mov ecx, 24
	mov edx, 0x534D4150
	mov [es:di + 20], dword 1
	int 0x15
	jc .failed

	cmp eax, edx
	jne .failed

	test ebx, ebx
	je .failed
	
.getentry:
	test ebx, ebx
	je .success ; ebx = 0 -> done
	mov eax, 0xE820
	mov ecx, 24
	mov edx, 0x534D4150

.checkentry:
; is the entry ok?

.addentry:

.success:
	mov [mmr+1], bp
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