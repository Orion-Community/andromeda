;
;    Low level memory map functions.
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

[SECTION .text]

[GLOBAL getmmr]
[EXTERN mmr]
; 
; This will return the mmr in eax. Used in mmap.c to get the mmr.
; 
getmmr:
	xor eax, eax
	mov eax, mmr
	ret

[GLOBAL updatecmosmmap]
updatecmosmmap:
	pushad
	mov eax, 1
	mov esi, 0x0
	mov ecx, 1<<20
	int 0x30	; returns amount of mem found in ecx
	push ecx

	mov eax, 1
	mov esi, 1<<20
	mov ecx, (1<<20)*63
	int 0x30

	mov edx, ecx
	pop ebx
	mov eax, 3
	int 0x30

	mov eax, 2
	int 0x30
	mov [mmr+4], cx
	mov [mmr], edx

	popad
	ret