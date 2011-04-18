;
;    Golden Eagle bootloader. Loads the fenix kernel.
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

[BITS 16]
[ORG 0X7C00]

; EXTERN kernelmain

main:
	mov si, hello
	call println

	call enable_A20

; setup GDT, A20 line

	or ax, ax
	jz loadedA20

panic:
	mov si, error
	call println
	jmp $

loadedA20:
	mov si, loadGDT
	call println
	jmp $
	
; switch to protected mode and 32 bits
;	call kernelmain


;
;  Output routines
; 

%include 'println.asm'

;
; enable A20 line
;

%include 'A20.asm'

;
; Some sort of data segment
;

	hello db 'Loading Golden Eagle bootloader', 0x0
	loadGDT db 'A20 Line enabled... Setting GDT.', 0x0
	error db 'Failed to load Golden Eagle succesfull...', 0x0
;
; End
;

times 512-($-$$)-2 db 0
dw 0xAA55

