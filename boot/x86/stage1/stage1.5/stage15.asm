;
;    Entry point for stage 1.5. This sector will use a dynamic sector loader to load the second stage and the micro kernel.
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
[SECTION .stage1]
[EXTERN endptr]

%include "boot/x86/include/masterboot.asmh"

jmp short main
nop

gdt:
    times 8 db 0
    UNREAL_SEG equ $ - gdt	; Data segment, read/write, expand down
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

main:
	mov di, 0x7c00
	mov si, OL_BUFOFF+OL_PART_TABLE
	push di
	mov cx, 0x40
	cld
	rep movsw
	push dx

	jmp .unrealmode

.bailout:
	mov si, failed
	call println
	cli
	jmp $

.unrealmode:
; before we load the core image and the second stage with the bios we
; have to hack the processor in unreal mode first.
	cli
	push ds	; save for later
	lgdt [gdtr]
	mov eax, cr0
	or eax, 1b
	mov cr0, eax

	mov ax, UNREAL_SEG
	mov ds, ax

	mov eax, cr0
	xor eax, 1b
	mov cr0, eax
	pop ds
	sti
; hello unreal mode

.loadcore:
	call calcsectors
	mov cx, ax
	mov eax, dword [0x7c00+8]
	add eax, 2	; third sector
	xor ebx, ebx

	mov bx, 0x7e0
	mov es, bx
	mov di, 0x200

	call int13_read
	jc .bailout

	pop dx
	pop si
	jmp 0x7E0:0x200

	jmp .bailout

;
; 16-bit bios disk interface
;

%include 'boot/x86/interface/disk.asm'

;
; Print routine
;

%include 'boot/x86/println.asm'
	
	failed db '0x2', 0x0

times 512 - ($ - $$) db 0
