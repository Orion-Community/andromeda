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

[GLOBAL proberam]
proberam:
	push ebp
	mov ebp, esp

	xor eax, eax
	in al, GEBL_PIC1_DATA
	push eax
	xor eax, eax
	in al, GEBL_PIC2_DATA
	push eax

	mov al, GEBL_PIC_DISABLE
	out GEBL_PIC1_DATA, al
	out GEBL_PIC2_DATA, al

	mov esi, GEBL_LOW_BASE	; begin at address 0x0
	xor ecx, ecx
	or esi, 0xffc	; last dword of block

.lowmem:
	memtest esi
	jne .lowend

	add esi, GEBL_PROBE_BLOCKSIZE	; add block for next test
	jmp .lowmem

.lowend:
	mov edx, ~0xffc	; mov edx, 0xffc
			; not edx
	and esi, edx	; round block down (block is rounded up)

	shr esi, 10	; esi /= 1024
	mov dx, si

	mov al, GEBL_CMOS_LOW_MEM_LOW_ORDER_REGISTER ; select the low order register
	out GEBL_CMOS_OUTPUT, al

	mov al, dl
	out GEBL_CMOS_OVERWRITE, al	; overwrite with better value

	mov al, GEBL_CMOS_LOW_MEM_HIGH_ORDER_REGISTER ; select the low order register
	out GEBL_CMOS_OUTPUT, al

	mov al, dh
	out GEBL_CMOS_OVERWRITE, al	; overwrite with better value
	call iowait

; prepare the mid mem loop
	mov cx, ((1<<20)*14)/0x1000	; max blocks untill hole
	mov esi, GEBL_EXT_BASE
	or esi, 0xffc	; last word of block

.midmem:
	memtest esi
	jne .midend
	add esi, GEBL_PROBE_BLOCKSIZE
	dec cx
	jz .midend	; we arrived at mem hole if the zf is set

	jmp .midmem

.midend:
	mov edx, ~0xffc
	and esi, edx
; 	sub esi, GEBL_EXT_BASE
	push esi

; prepare high mem
	mov ecx, ((1<<20)*48)/0x1000	; probe for max 63mb extended memory
					; making a total of 64mb
	mov esi, GEBL_HIGH_BASE
	or esi, 0xffc

.highmem:
	memtest esi
	jne .highend
	add esi, GEBL_PROBE_BLOCKSIZE
	dec ecx
	jz .highend
	jmp .highmem

.highend:
	mov edx, ~0xffc
	and esi, edx
	sub esi, GEBL_HIGH_BASE
	pop edx
	add edx, esi

	shr edx, 10

	mov al, GEBL_CMOS_EXT_MEM_LOW_ORDER_REGISTER ; select the low order register
	out GEBL_CMOS_OUTPUT, al

	mov al, dl
	out GEBL_CMOS_OVERWRITE, al	; overwrite with better value

	mov al, GEBL_CMOS_EXT_MEM_HIGH_ORDER_REGISTER ; select the low order register
	out GEBL_CMOS_OUTPUT, al

	mov al, dh
	out GEBL_CMOS_OVERWRITE, al
	call iowait
	call cmosmmap

.done:
	pop eax		; re-set the old pic masks
	out GEBL_PIC2_DATA, al
	pop eax
	out GEBL_PIC1_DATA, al

	pop ebp
	ret

cmosmmap:
	mov edi, GEBL_MMR_POINTER

	call cmoslowmmap
	jc .failed

	nxte

.highmem:
	mov al, GEBL_CMOS_EXT_MEM_LOW_ORDER_REGISTER
	out GEBL_CMOS_OUTPUT, al	; tell the cmos we want to read the high memory
	call iowait

	xor ax, ax
	in al, GEBL_CMOS_INPUT ; get the low bytes
	push ax

	mov al, GEBL_CMOS_EXT_MEM_HIGH_ORDER_REGISTER
	out GEBL_CMOS_OUTPUT, al	; get most significant byte
	call iowait

	xor ax, ax
	in al, GEBL_CMOS_INPUT

	pop dx
	shl ax, 8
	or ax, dx	; ax is the most significant byte, dx the least significant

	and eax, 0xffff
	shl eax, 10

	call addmemoryhole

	jmp .done

.failed:
	popfd
	stc
	ret

.done:
	add cx, 2	; lowmmap entries
	mov [mmr+4], cx	
	clc
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