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
	xor eax, eax
	in al, GEBL_PIC1_DATA
	shl ax, 8
	in al, GEBL_PIC2_DATA
	push eax

	mov al, GEBL_PIC_DISABLE
	out GEBL_PIC1_DATA, al
	out GEBL_PIC2_DATA, al

	mov ebp, esi	; esi = starting address
	or esi, 0xfff
	and esi, ~0xfff
	push esi	; esi = starting address rounded (down) to block boundry

	sub ebp, esi	; bytes to add due to rounding down
	add ecx, ebp
	shr ecx, 12
	mov eax, ecx	; eax is blocks to test
	xor ecx, ecx
	or esi, 0xffc	; round up to last word of block

.looptop:
	memtest esi
	jne .end
	add esi, GEBL_PROBE_BLOCKSIZE
	add ecx, GEBL_PROBE_BLOCKSIZE	; byte counter

	dec eax
	jz .end
	jmp .looptop

.end:
	pop ebx	; starting address
	pop eax		; re-set the old pic masks
	out GEBL_PIC2_DATA, al
	shr ax, 8
	out GEBL_PIC1_DATA, al

	pop edx
	pop eax

	mov ebp, esp
	mov [ebp+20], ebx
	mov [ebp+24], ecx

	push eax
	jmp edx
