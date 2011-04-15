;
;    Text output routines. These can only be used in 16 bit real mode. These are used to display the first messages.
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

;    You should have received a copy of the GNU General Public License
;    along with this program.  If not, see <http://www.gnu.org/licenses/>.
;

println:
	xor cx, cx ; strlen
	xor dx, dx

putchar:
	lodsb
	or al, al
	jz newline 			; 0 byte found
	mov ah, 0x0E 			; teletype output
	xor bh, bh			; page 0
	int 0x10
	inc cx
	jmp putchar

newline:
	xor bh, bh
	mov al, 0x0A
	mov ah, 0x0E
	int 0x10

endprintln:
	xor bh, bh
	mov al, 0x08 ; backspace
	inc dx
	int 0x10
	cmp cx, dx
	jne endprintln ; not at beginning yet

	retn