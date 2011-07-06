;
;    This IRQ (25) serves the user some memory related services. Such as a ram prober,
;    a cmos update routine and a function which builds a mmap from the cmos.
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

[SECTION .data]

[SECTION .text]
	gdt qword 0
	idt qword 0

[GLOBAL setvidmode]
setvidmode:
; void setvidmode(uint8_t mode)
	pushad
	pushfd

	sgdt gdt
	sidt idt

	mov eax, cr0
	and eax, 0x7FFFFFFE	; disable the PE and paging bit
	mov cr0, eax

	xor ax, ax
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov fs, ax
	mov gs, ax
	jmp 0x0:.flush
.flush1:
	; in pmode now..
	
.end:
	lgdt gdt
	lidt idt
	mov eax, cr0
	xor eax, (1<<31)|1 ; 0x80000001
	mov cr0, eax
	popfd
	popad