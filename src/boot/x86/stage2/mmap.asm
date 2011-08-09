;
;    Get the system memory map from the bios and store it in a global constant.
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
[SECTION .stage2]

%include "mmap.asmh"

; Create a multiboot memory map. It is not multiboot compatible in the way it rolls out of this function. The size still has to be added into the entry.
; The entry size is given in the mmr.
[GLOBAL getmemorymap]
getmemorymap:
	mov ax, OL_MMAP_SEG
	mov es, ax
	xor di, di

	and eax, 0xffff
	shl eax, 4 ; eax * 16
	movzx ebx, di
	add eax, ebx
	mov [mmr], eax
jmp mm_88
; 
; The memory map returned from bios int 0xe820 is a complete system map, it will be given to the bootloader kernel for little editing
;
%ifndef __OLDPC
mm_e820:
	push bp
	xor bp, bp ; entry counter
	mov eax, 0xe820
	xor ebx, ebx
	mov ecx, 0x18
	mov edx, OL_SMAP
	push edx
	mov [es:di+20], dword OL_ACPI ; acpi 3.x compatibility
	int 0x15
	pop edx	; restore magic number

	jc .failed
	cmp eax, edx ; magic word should also be in eax after interrupt
	jne .failed
	test ebx, ebx ; ebx = 0 means the list is only 1 entry long = worthless
	jz .failed
	jmp .addentry

.getentry:
	mov eax, 0xe820
	mov ecx, 0x18
	mov edx, OL_SMAP
	push edx
	mov [es:di+20], dword OL_ACPI
	int 0x15
	pop edx

	jc .done ; carriage means end of list
	cmp eax, edx
	jne .failed

.addentry:
	jcxz .skipentry ; entries with length 0 are compleet bullshit
	add di, 24
	inc bp

.skipentry:
	test ebx, ebx ;
	jnz .getentry

.done:
	mov [mmr+4], bp
	pop bp
	clc	; clear carry flag
	ret

.failed:
	pop bp
	call mm_e801
	stc

	ret
%endif

;
; This memory map will contain 4 entries. See doc/mmap.txt for more information.
;
mm_e801:
	xor di, di
	mov ax, OL_MMAP_SEG
	mov es, ax
	call lowmmap
	jc .failed

	nxte .midmem

.midmem:
	mov ax, 0xe801
	int 0x15
	jc .failed
	push bx		; save bx and dx for the high mem entry
	push dx

	and ecx, 0xffff	; clear upper 16 bits
	shl ecx, 10
	mov [es:di], dword OL_EXT_BASE
	mov [es:di+8], ecx		;dword (0x3c00<<10)
	mov [es:di+16], byte OL_USABLE_MEM
	mov [es:di+20], byte OL_ACPI

	jecxz .useax
	nxte .highmem

.useax:
	and eax, 0xffff
	shl eax, 10
	mov [es:di+8], eax
	nxte .highmem

.highmem:
	pop dx
	pop bx

	and edx, 0xffff
	shl edx, 16	; edx * 1024 * 64
	mov [es:di], dword OL_HIGH_BASE
	mov [es:di+8], edx
	mov [es:di+16],	byte OL_USABLE_MEM	; type -> usable
	mov [es:di+20], byte OL_ACPI	; acpi 3.0 compatible

	test edx, edx
	jz .usebx
	jmp .done

.usebx:
	and ebx, 0xffff
	shl ebx, 16
	test ebx, ebx
	jz .failed
	mov [es:di+8], ebx
	jmp .done

.failed:
	jmp mm_88
	stc
	ret

.done:
	mov [mmr+4], word 0x4	; 2x low mem + mid mem + high mem = 4 entries
	clc
	ret

; 
; This is a function from the dinosaur time, it it used on verry old PC's when all other methods to get a memory map fail. If this
; fails to, the bootloader will cry to the user, and tell him to buy a new pc.
; 
mm_88:
	xor di, di	; set segment:offset of the buffer, just to be sure
	mov ax, OL_MMAP_SEG
	mov es, ax

	call lowmmap	; get a lowmmap
	jc .failed
	
	nxte .highmem

.highmem:
	mov ax, 0x8800
	int 0x15
	jc .failed
	and eax, 0xffff
	shl eax, 10

	call addmemoryhole

	jmp .done

.failed:
	stc
	ret
.done:
	add cx, 2	; low entries
	mov [mmr+4], cx
	clc
	ret

;
; This routine makes a memory map of the low memory (memory < 1M). When it returns, es:di will point to the start of the last entry.
;
lowmmap:
; low available memory
	call copy_empty_entry	; copy first entry
	xor ax, ax
	int 0x12	; get low memory size

	and eax, 0xffff	; clear upper 16  bits
	shl eax, 10	; convert to bytes
	push eax	; save for later
	mov [es:di], dword OL_LOW_BASE
	mov [es:di+8], eax
	mov [es:di+16], dword OL_USABLE_MEM
	mov [es:di+20], dword OL_ACPI	; acpi 3.0 compatible entry

	nxte .lowres

.lowres:
; low reserver memory
	pop eax
	and edx, 0xffff
	mov edx, (1 << 20)
	sub edx, eax

	mov [es:di], eax
	mov [es:di+8], edx	; length (in bytes)
	mov [es:di+16], dword OL_RESERVED_MEM	; reserverd memory
	mov [es:di+20], dword OL_ACPI		; also this entry is acpi 3.0 compatible
	jmp .done

.failed:
	stc
	ret
.done:
	clc
	ret

; 
; Provide the amount of extended memory in eax (in bytes).
; 
; Returns total added entries in cx and es:di points to the start of the last entry.
; 
addmemoryhole:
	pushad	; save all registers
	xor cx, cx
	push cx	; .next will pop the counter off

	call copy_empty_entry
	cmp eax, (15 << 20) ; 15 mb in bytes
	jb .remainder
	
	; first 14 mb are usable
	mov [es:di], dword 0x100000	; base - 15mb
	mov [es:di+8], dword 0x00E00000 ; length = 14 mb
	mov [es:di+16], dword OL_USABLE_MEM	; usable memory
	mov [es:di+20], dword OL_ACPI	; acpi 3.0
	
	call .next
	
.hole:	
	mov [es:di], dword 0x00F00000	; base - 15mb
	mov [es:di+8], dword 0x00100000	; length = 1mb
	mov [es:di+16], dword OL_BAD_MEM	; bad memory
	mov [es:di+20], dword OL_ACPI	; acpi 3.0
	
	sub eax, (15 << 20)	; substract 15 mb (the reserved mem + the memory already defined as usable) from it
	pop cx
	inc cx
	push cx

	test eax, eax
	jz .done

	pop cx
	dec cx
	push cx
	call .next

.remainder:
	mov [es:di], dword 0x001000000	; base
	mov [es:di+8], eax
	mov [es:di+16], dword OL_USABLE_MEM	; free memory
	mov [es:di+20], dword OL_ACPI	; acpi 3.0
	pop cx
	inc cx
	push cx	; to prevent stack corruption

.done:
	pop cx
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
	pop dx	; return addr
	pop cx
	inc cx	; entry counter
	push cx
	nxte dx

; 
; This routine copies an empty memory map to the location specified by es:di
; 
copy_empty_entry:	; this subroutine copies an emty memory map to the location specified by es:di
	cld	; just to be sure that di gets incremented
	mov si, mmap_entry
	mov cx, 0xc
	rep movsw	; copy copy copy!
	sub di, 0x18	; just to make addressing esier
	ret
; now there is an empty entry at [es:di]
; times 1024 - ($ - $$) db 0
