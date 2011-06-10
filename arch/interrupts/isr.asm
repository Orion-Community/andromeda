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

isrStub:
	pushad
	mov ebp, esp
	mov eax, [ebp+32]

	mov dx, ds
	push dx
	mov dx, 0x10	; kernel ring

	mov ds, dx
	mov es, dx
	mov fs, dx
	mov gs, dx

	call eax

	pop dx
	mov ds, dx
	mov es, dx
	mov gs, dx
	mov es, dx

	popad
	add esp, 8	; pop error num + routine
	sti
	iret	; interrupt return

%macro isrNoError 1
	cli
	push 0
	push %1
%endmacro