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
	counter dd 0

[SECTION .text]

%include "/home/michel/osdev/goldeneaglebl/arch/mm/include/proberam.h"

[EXTERN printnum]
[EXTERN pic_eoi]

[GLOBAL proberam]
proberam:
	push ebp
	mov ebp, esp

; 	xor ax, ax
; 	in 

	mov esi, 0x1000000	; begin at address 0x0
	xor ecx, ecx	; 256 blocks to test

.lowmem:
	or esi, 0xffc	; last dword of block
	mov eax, dword [esi]	; original value at address
	mov edx, eax		; copy
	not eax			; invert eax
	mov dword [esi], eax
	mov dword [dummy], edx	; dummy write
	wbinvd	; write back and invalidate the cache
	mov ebx, dword [esi] ; get value back
	mov dword [esi], edx

	cmp ebx, eax
	jne .lowend

	add ecx, 0x1000

	add esi, 0x1000	; add block for next test
	jmp .lowmem

.lowend:

	push dword 0
	push dword 0
	push dword 16
	push ecx
	call printnum
	add esp, 4*4

.done:
	pop ebp
	ret