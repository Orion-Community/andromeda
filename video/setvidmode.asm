;
;    Adjust video mode to user input.
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

%macro stsgms 1
	mov ax, %1
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov fs, ax
	mov gs, ax
%endmacro

[SECTION .data]

[SECTION .text]
	pmGDT qword 0
	PMidt qword 0

gdt:
    times 8 db 0
    CODE_SEG equ $ - gdt	; Code segment, read/execute, nonconforming
        dw 0FFFFh
        dw 0
        db 0
        db 0x9A
        db 0xF
        db 0
    DATA_SEG equ $ - gdt	; Data segment, read/write
        dw 0FFFFh
        dw 0
        db 0
        db 0x9A
        db 0xF
        db 0
gdt_end equ $ - gdt

gdtr:
	dw gdt_end - 1
	dd gdt

idtr:
	dw 0x3ff
	dd 0x0

[GLOBAL setvidmode]
setvidmode:
; void setvidmode(uint8_t mode)
	pushad
	pushfd

cli

	sgdt PMgdt
	sidt PMidt

	mov eax, cr0
	and eax, 0x7fffffff	; disable the page bit
	mov cr0, eax

	lgdt [gdtr]	; load real mode gdt
	jmp CODE_SEG:.flush
.flush:
	mov ax, CODE_SEG	; flush all segments
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov fs, ax
	mov gs, ax

	lidt [idtr]

	; now we will disable the pe bit
	mov eax, cr0
	and eax, 0x7FFFFFFE
	mov cr0, eax
	jmp 0x0:.flush2

.flush2:
	stsgms 0	; set all segments to 0
	mov sp, 0x7300	; real mode stack
	sti ; here we are.. in real mode => goodluck

rm_main:

.end:
	lgdt gdt
	lidt idt
	mov eax, cr0
	xor eax, (1<<31)|1 ; 0x80000001
	mov cr0, eax
	popfd
	popad
	ret
