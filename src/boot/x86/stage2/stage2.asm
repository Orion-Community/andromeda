;
;    Entry point for the second stage. This wil also execute the micro kernel.
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
[ORG 0x8200]
jmp short main

%include "boot/stage15.asmh"

;
; GLOBAL DESCRIPTOR TABLE
;
gdt:
    times 8 db 0
    CODE_SEG equ $ - gdt	; Code segment, read/execute, nonconforming
        dw 0FFFFh
        dw 0
        db 0
        db 0x9A
        db 0xCF
        db 0
    DATA_SEG equ $ - gdt	; Data segment, read/write, expand down
        dw 0FFFFh
        dw 0
        db 0
        db 0x92
        db 0xCF
        db 0
gdt_end equ $ - gdt

gdtr:
	dw gdt_end - 1; gdt limit = size
	dd gdt ; gdt base address


	; Status messages
	pmodemsg db 'Implementing a GDT and PMode.', 0x0
	failed db '0x2', 0x0


main:
%ifndef __PROBERAM
	call getmemorymap
	jc .bailout
%endif

	mov si, pmodemsg
	call println

	cli
	lgdt [gdtr]
	mov eax, cr0
	or eax, 00001b 	; enable pe bit
	mov cr0, eax
.flush:
	mov ax, DATA_SEG	; flush segments
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov esp, 0x8000; top of usable memory..
	jmp32 0x8, linearaddr(0x0, pmodemain)	; get to pmode



.bailout:
	cli
	jmp $

;
; Print routines
;

%include 'println.asm'

%include 'mmap.asm'

%include 'pmodemain.asm'

times 1024 - ($-$$) db 0
