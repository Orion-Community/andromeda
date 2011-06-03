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

[GLOBAL mmr]
getmemorymap:
jmp e801
e820:
	push bp
	push word 0x50
	pop es
	xor di, di	; destination pointer

	mov [mmr], es		; store the segment
	mov [mmr+2], di		; store the offset

	xor bp, bp ; entry counter
	mov eax, 0xE820
	xor ebx, ebx
	mov ecx, 0x18
	mov edx, 0x534D4150
	push edx
	mov [es:di+20], dword 1 ; acpi 3.x compatibility
	int 0x15
	pop edx

	jc .failed
	cmp eax, edx ; magic word should also be in eax after interrupt
	jne .failed
	test ebx, ebx ; ebx = 0 means the list is only 1 entry long = worthless
	jz .failed
	jmp .addentry

.getentry:
	mov eax, 0xE820
	mov ecx, 0x18
	mov edx, 0x534D4150
	push edx
	mov [es:di+20], dword 1
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
	or ebx, ebx ;
	jnz .getentry

.done:
	mov [mmr+4], word bp
	pop bp
	clc	; clear carry flag
	ret
.failed:
	pop bp
	stc 	; set the carry flag
	ret

e801:
	mov ax, 0x50
	mov es, ax
	xor di, di
	mov word [mmr], ax
	mov word [mmr+2], di
	mov dx, .lowmem

.copy_empty_entry:	; this subroutine copies an emty memory map to the location specified by es:di
	cld	; just to be sure that di gets incremented
	mov si, mmap_entry
	mov cx, 0xc
	rep movsw
	sub di, 0x18
	jmp dx
; now there is an empty entry at [es:di]

.lowmem:
	xor ax, ax
	int 0x12	; get low memory size
	jc .failed	; if interrupt 0x12 is not support, its really really over..
	push ax
	mov [es:di], dword 0x0
	mov [es:di+8], ax
	mov [es:di+16], dword 0x1
	mov [es:di+20], dword 0x1	; acpi 3.0 compatible entry
	mov dx, .lowres
	jmp .next

.lowres:
	pop ax
	mov [es:di], ax
	mov dx, (1 << 20)
	sub dx, ax
	mov [es:di+8], dx
	mov [es:di+16], dword 0x2	; reserverd memory
	mov [es:di], dword 0x1
	mov dx, .midmem
	jmp .next

.midmem:
; 	mov ax, 0xe801
; 	int 0x15
	jmp .done

.highmem:

.next:
	add di, 0x18
	jmp .copy_empty_entry

.failed:
	stc
	ret

.done:
	clc
	ret

mmap_entry:	; 0x18-byte mmap entry
	base dq 0	; base address
	length dq 0	; length (top_addr - base_addr)
	type dd 0	; entry type
	acpi3 dd 0	; acpi 3.0 compatibility => should be 1
	mmap_size equ $ - mmap_entry

mmr:
	dd 0 		; dw 0 -> segment
			; dw 0 -> offset
	dw 0 ;entry count
	db 24 ; entry size