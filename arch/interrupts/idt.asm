;
;    IDT loader.
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

[GLOBAL installEntry]
installEntry:
	push ebp
	mov ebp, esp

	mov esi, idtentry
	mov edi, 0x7800
	add edi, [ebp+8]
	mov ecx, 0x4
	rep movsw
	sub edi, 0x8
	; edi points to the beginning of the new

	pop ebp
	ret

[GLOBAL loadidt]
loadidt:
	push ebp
	mov ebp, esp

	mov eax, dword [ebp+8]
	lidt [eax]

	pop ebp
	ret

idtr:
	dw 0	; limit
	dd 0	; base

idtentry:
	dw 0	; low base
	dw 0	; selector
	db 0	; should always be 0
	db 0	; flags
	dw 0	; high base