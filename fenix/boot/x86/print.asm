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
;
;    You should have received a copy of the GNU General Public License
;    along with this program.  If not, see <http://www.gnu.org/licenses/>.
;

print:
	lodsb
	or al, al
	jz .return
	mov ah, 0x0E
	xor bh, bh	; page 0
	int 0x10
	jmp print

.return:
	mov al, 0x0A
	mov ah, 0x0E
	xor bh, bh
	int 0x10

	mov al, 0x0D
	mov ah, 0x0E
	xor bh, bh
	int 0x10
	ret