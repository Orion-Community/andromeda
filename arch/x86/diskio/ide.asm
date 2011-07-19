;
;    This piece of code is dedicated to the ide driver -> reading/writing to the disk.
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

[GLOBAL ide_init]
ide_init:
	push ebp
	mov ebp, esp

	mov ebx, 0x7c00
	test byte [ebx], 0x80
	jz .fail

.end:
	pop ebp
	ret

.fail:
	jmp $