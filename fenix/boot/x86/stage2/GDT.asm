;
;    The General Descriptor Table.
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

;
; The GDT table
;

gdt:
	db 0
	db 0

	GDT_CODE_SEL equ $-gdt ; code selector
		dw 0xFFFF ;limit
		dw 0	  ; base 15:0
		db 0	  ; base 23:16
		db 0x9A   ; type -> ring 0 code and readable
		db 0xCF   ; page granular, 32 bit
		db 0	  ; base 31:24

	GDT_DATA_SEL equ $-gdt ; code selector
		dw 0xFFFF ;limit
		dw 0	  ; base 15:0
		db 0	  ; base 23:16
		db 0x92   ; type -> ring 0 code and writable
		db 0xCF   ; page granular, 32 bit
		db 0	  ; base 31:24
gdt_end: ; to calc size

gdtr:
	dw gdt - gdt_end - 1 ; gdt limit = size
	db gdt ; gdt base address



