;
;    Implementation of input output routines.
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

[GLOBAL inb]
[GLOBAL outb]
[GLOBAL testmmr]
[EXTERN mmr]
[SECTION .text]
inb:
	push ebp
	mov ebp, esp
	
	xor eax, eax
	mov dx, word [ebp+8] ; port
	in al, dx

	pop ebp
	ret

outb:
	push ebp
	mov ebp, esp
	
	mov al, byte [ebp+8] ; data
	mov dx, word [ebp+12] ; port
	out dx, al

	pop ebp
	ret
