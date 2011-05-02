;
;    Text output using the bios. WARNING: DO NOT USE IF YOU ARE NOT IN REAL MODE.
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

println:
	lodsb
	or al, al
	jz .return	; null byte found
	mov ah, 0x0E
	xor bh, bh	; page 0
	int 0x10
	jmp println

.return:
	mov al, 0x0A 	; new line
	mov ah, 0x0E
	xor bh, bh
	int 0x10

	mov al, 0x0D	; carrage return
	mov ah, 0x0E
	xor bh, bh
	int 0x10
	ret