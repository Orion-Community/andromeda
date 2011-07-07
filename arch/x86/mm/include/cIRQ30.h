;
;    IRQ30 header.
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

%ifndef __IRQ25_H
%define __IRQ25_H

%define GEBL_MMR_POINTER 0x500

%define GEBL_EXT_BASE 0x00100000
%define GEBL_HIGH_BASE 0x001000000
%define GEBL_LOW_BASE 0x00
%define GEBL_HOLE_BASE 0xf00000
%define GEBL_ACPI 0x01
%define GEBL_PROBE_BLOCKSIZE 0x1000

; CMOS i/o ports

%define GEBL_CMOS_OUTPUT 0x70
%define GEBL_CMOS_INPUT 0x71
%define GEBL_CMOS_OVERWRITE GEBL_CMOS_INPUT

; CMOS data registers
%define GEBL_CMOS_EXT_MEM_LOW_ORDER_REGISTER 0x30
%define GEBL_CMOS_EXT_MEM_HIGH_ORDER_REGISTER 0x31
%define GEBL_CMOS_LOW_MEM_LOW_ORDER_REGISTER 0x15
%define GEBL_CMOS_LOW_MEM_HIGH_ORDER_REGISTER 0x16
%define GEBL_NMI_DISABLE 0x1


; ram types
%define GEBL_USABLE_MEM 0x1
%define GEBL_RESERVED_MEM 0x2
%define GEBL_RECLAIMABLE_MEM 0x3
%define GEBL_NVS_MEM 0x4
%define GEBL_BAD_MEM 0x5

; pic ports
%define GEBL_PIC1_COMMAND 0x20
%define GEBL_PIC2_COMMAND 0xa0

%define GEBL_PIC1_DATA 0x21
%define GEBL_PIC2_DATA 0xa1

; PIC commands
%define GEBL_PIC_DISABLE 0xff

%macro nxte 1
	add edi, 0x18
	cld	; just to be sure that di gets incremented
	mov esi, mmap_entry
	mov cx, 0xc
	rep movsw	; copy copy copy!
	sub edi, 0x18	; just to make addressing esier

	jmp %1
%endmacro

%macro nxte 0
	add edi, 0x18
	cld	; just to be sure that di gets incremented
	mov esi, mmap_entry
	mov cx, 0xc
	rep movsw	; copy copy copy!
	sub edi, 0x18	; just to make addressing esier
%endmacro

%macro memtest 1
	push eax
	push ebx
	push edx

	mov eax, dword [%1]	; original value at address
	mov edx, eax		; copy
	not eax			; invert eax
	mov dword [%1], eax
	mov dword [dummy], edx	; dummy write
	wbinvd	; write back and invalidate the cache
	mov ebx, dword [%1] ; get value back
	mov dword [%1], edx
	xor eax, ebx	; zf is set when result is equal

	pop edx
	pop ebx
	pop eax
%endmacro

%macro xtest 2
	push %1
	xor %1, %2
	pop %1
%endmacro

dummy dd 0

mmap_entry:	; 0x18-byte mmap entry
	base dq 0	; base address
	length dq 0	; length (top_addr - base_addr)
	type dd 0	; entry type
	acpi3 dd 0	; acpi 3.0 compatibility => should be 1

[EXTERN mmr]
%endif
