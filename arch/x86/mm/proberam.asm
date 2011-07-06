;
;    If all 4 methods of getting a memory map failed (and thus the bios is bugged) we can use 
;    this to get info about the available memory.
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
	dummy dd 0

%include "arch/x86/mm/include/proberam.h"

[SECTION .text]

[EXTERN printnum]
[EXTERN iowait]

[GLOBAL cIRQ80]
cIRQ80:
	mov ebp, esp
	push ebp
	mov eax, [ebp+36]

	xtest eax, 0001b
	jz proberam

	xtest eax, 0010b
	jz createmmap
	
	xtest eax, 0011b
	jz updatecmos

.end:
	pop ebp
	ret

; 
; This function probe's for ram. It expects a starting address in esi and the amount of bytes to probe for in ecx (note that the minimum size is 4kb). When it returns,
; esi contains the correct (blockrounded (rounded down)) starting address and ecx the amount of bytes found.
; 
proberam:
	xor eax, eax
	in al, GEBL_PIC1_DATA
	shl ax, 8
	in al, GEBL_PIC2_DATA
	push eax

	mov al, GEBL_PIC_DISABLE
	out GEBL_PIC1_DATA, al
	out GEBL_PIC2_DATA, al

	mov ebp, esi	; esi = starting address
	and esi, ~0xfff	; round down to block
	push esi	; esi = starting address rounded (down) to block boundry

	sub ebp, esi	; bytes to add due to rounding down
	add ecx, ebp
	shr ecx, 12
	jz .end		; no blocks to test. lets get out.

	mov eax, ecx	; eax is blocks to test
	xor ecx, ecx
	or esi, 0xffc	; round up to last word of block
	push .l1
.looptop:
	cmp esi, GEBL_HOLE_BASE | 0xffc
	je .skiphole
.l1:
	memtest esi
	jne .end	; xor sets zf if equal
	add esi, GEBL_PROBE_BLOCKSIZE
	add ecx, GEBL_PROBE_BLOCKSIZE	; byte counter

	dec eax
	jz .end
	jmp .looptop

.skiphole:
; 	xor esi, GEBL_HIGH_BASE | 0xf0000	; add esi, 1<<20
	sub eax, (1<<20)/GEBL_PROBE_BLOCKSIZE
	add ecx, 1<<20
	jmp [esp]

.end:
	add esp, 4	; pop .l1 label
	pop ebx	; starting address
	pop eax		; re-set the old pic masks
	out GEBL_PIC2_DATA, al
	shr ax, 8
	out GEBL_PIC1_DATA, al

	pop ebp
	mov [ebp+24], ebx
	mov [ebp+32], ecx
.done:
	ret

;
; Get a memory map from the cmos. Retuns amount of entries in ecx and a pointer to the first entry in edx.
;
createmmap:
	mov edi, GEBL_MMR_POINTER

	call cmoslowmmap
	jc .failed

	nxte

.highmem:
	mov al, GEBL_CMOS_EXT_MEM_LOW_ORDER_REGISTER
	out GEBL_CMOS_OUTPUT, al	; tell the cmos we want to read the high memory
	call iowait

	xor eax, eax
	in al, GEBL_CMOS_INPUT ; get the low bytes
	push eax

	mov al, GEBL_CMOS_EXT_MEM_HIGH_ORDER_REGISTER
	out GEBL_CMOS_OUTPUT, al	; get most significant byte
	call iowait

	xor ax, ax
	in al, GEBL_CMOS_INPUT

	pop edx
	shl ax, 8
	or ax, dx	; ax is the most significant byte, dx the least significant

	and eax, 0xffff
	shl eax, 10

	call addmemoryhole

	jmp .done

.failed:
	pop ebp
	mov [ebp+32], dword 0
	mov [ebp+28], dword GEBL_MMR_POINTER
	ret

.done:
	add ecx, 2	; lowmmap entries
	mov [mmr+4], cx
	pop ebp
	mov [ebp+32], ecx
	mov [ebp+28], dword GEBL_MMR_POINTER

	ret

cmoslowmmap:
	call copy_empty_entry

	mov al, GEBL_CMOS_LOW_MEM_LOW_ORDER_REGISTER ; get least sig byte
	out GEBL_CMOS_OUTPUT, al
	call iowait	; wait

	xor ax, ax
	in al, GEBL_CMOS_INPUT
	push ax	 ; sava data temp
	
	mov al, GEBL_CMOS_LOW_MEM_HIGH_ORDER_REGISTER ; most sig byte
	out GEBL_CMOS_OUTPUT, al
	call iowait

	xor ax, ax
	in al, GEBL_CMOS_INPUT	; collect data
	
	pop dx
	shl ax, 8	; put al in ah
	or ax, dx ; ah is the most significant byte, dl the least significant

	and eax, 0xffff
	shl eax, 10	; eax*1024 -> convert to bytes
	push eax	; save for the low reserved mmap
	
	mov [es:edi], dword GEBL_LOW_BASE
	mov [es:edi+8], eax
	mov [es:edi+16], dword GEBL_USABLE_MEM
	mov [es:edi+20], dword GEBL_ACPI	; acpi 3.0 compatible entry
	nxte

.lowres:
; low reserver memory
	pop eax	; get saved value
	and edx, 0xffff
	mov edx, (1 << 20)
	sub edx, eax

	mov [es:edi], eax
	mov [es:edi+8], edx	; length (in bytes)
	mov [es:edi+16], dword GEBL_RESERVED_MEM	; reserverd memory
	mov [es:edi+20], dword GEBL_ACPI		; also this entry is acpi 3.0 compatible
	jmp .done

.done:
	clc
	ret

addmemoryhole:
	pushad	; save all registers
	xor ecx, ecx
	push ecx	; .next will pop the counter off

	call copy_empty_entry
	cmp eax, (15 << 20) ; 15 mb in bytes
	jb .remainder
	
	; first 14 mb are usable
	mov [es:edi], dword 0x100000	; base - 15mb
	mov [es:edi+8], dword 0x00E00000 ; length = 14 mb
	mov [es:edi+16], dword GEBL_USABLE_MEM	; usable memory
	mov [es:edi+20], dword GEBL_ACPI	; acpi 3.0
	
	call .next
	
.hole:	
	mov [es:edi], dword 0x00F00000	; base - 15mb
	mov [es:edi+8], dword 0x00100000	; length = 1mb
	mov [es:edi+16], dword GEBL_BAD_MEM	; bad memory
	mov [es:edi+20], dword GEBL_ACPI	; acpi 3.0
	
	sub eax, (15 << 20)	; substract 15 mb (the reserved mem + the memory already defined as usable) from it
	pop ecx
	inc ecx
	push ecx

	test eax, eax
	jz .done

	pop ecx
	dec ecx		; if not decreased it wil increase twice.
	push ecx
	call .next

.remainder:
	mov [es:edi], dword 0x001000000	; base
	mov [es:edi+8], eax
	mov [es:edi+16], dword GEBL_USABLE_MEM	; free memory
	mov [es:edi+20], dword GEBL_ACPI	; acpi 3.0
	pop ecx
	inc ecx
	push ecx	; to prevent stack corruption

.done:
	pop ecx
	add esp, 4	; pop edi (change in di (mmap offset) should not be reverted)
	pop esi
	pop ebp
	add esp, 4	; pop esp
	pop ebx
	pop edx
	add esp, 4	; pop ecx - we kept a counter in cx, should not be reverted
	pop eax
	ret

.next:
	pop edx	; return addr
	pop ecx
	inc ecx	; entry counter
	push ecx
	push edx
	nxte
	ret

copy_empty_entry:	; this subroutine copies an emty memory map to the location specified by es:edi
	cld	; just to be sure that edi gets incremented
	mov si, mmap_entry
	mov cx, 0xc
	rep movsw	; copy copy copy!
	sub edi, 0x18	; just to make addressing esier
	ret

; 
; Update the cmos memory registers. Amount of low memory in bytes in ebx and the amount of extended memory (in bytes) in edx.
; 
updatecmos:
	mov ebx, dword [ebp+24]
	mov edx, dword [ebp+28]	; extended memory above 1M (formot 2^n-1)
.lowmem:
	shr ebx, 10

	mov al, (GEBL_NMI_DISABLE<<7) | GEBL_CMOS_LOW_MEM_LOW_ORDER_REGISTER ; select the low order register
	out GEBL_CMOS_OUTPUT, al

	mov al, bl
	out GEBL_CMOS_OVERWRITE, al	; overwrite with better value

	mov al, (GEBL_NMI_DISABLE<<7) | GEBL_CMOS_LOW_MEM_HIGH_ORDER_REGISTER ; select the low order register
	out GEBL_CMOS_OUTPUT, al

	mov al, bh
	out GEBL_CMOS_OVERWRITE, al	; overwrite with better value
	call iowait

.highmem:
	and edx, 0x3f00000
	shr edx, 10

	mov al, (GEBL_NMI_DISABLE<<7) | GEBL_CMOS_EXT_MEM_LOW_ORDER_REGISTER ; select the low order register
	out GEBL_CMOS_OUTPUT, al

	mov al, dl
	out GEBL_CMOS_OVERWRITE, al	; overwrite with better value

	mov al, (GEBL_NMI_DISABLE<<7) | GEBL_CMOS_EXT_MEM_HIGH_ORDER_REGISTER ; select the low order register
	out GEBL_CMOS_OUTPUT, al

	mov al, dh
	out GEBL_CMOS_OVERWRITE, al
	call iowait

.end:
	pop ebp
	mov [ebp+32], dword 1
	ret