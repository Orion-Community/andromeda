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

	xor ax, ax
	in al, GEBL_PIC1_DATA
	push ax
	xor ax, ax
	in al, GEBL_PIC2_DATA
	push ax

	mov al, GEBL_PIC_DISABLE
	out GEBL_PIC1_DATA, al
	out GEBL_PIC2_DATA, al

	mov esi, GEBL_LOW_BASE	; begin at address 0x0
	xor ecx, ecx	; 256 blocks to test
	or esi, 0xffc	; last dword of block

.lowmem:
	memtest esi
	jc .lowend

	add esi, GEBL_PROBE_BLOCKSIZE	; add block for next test
	jmp .lowmem

.lowend:
	mov edx, ~0xffc	; mov edx, 0xffc
			; not edx
	and esi, edx	; round block down (block is rounded up)

; %ifdef __DEBUG
; 	push dword 0
; 	push dword 0
; 	push dword 16
; 	push esi
; 	call printnum
; 	add esp, 4*4
; %endif
	shr esi, 10	; esi /= 1024
	xor edx, edx
	or dx, si	; mov ax, si

	mov al, GEBL_CMOS_LOW_MEM_LOW_ORDER_REGISTER ; select the low order register
	out GEBL_CMOS_OUTPUT, al
	call iowait

	mov al, dl
	out GEBL_CMOS_OVERWRITE, al	; overwrite with better value
	call iowait

	mov al, GEBL_CMOS_LOW_MEM_HIGH_ORDER_REGISTER ; select the low order register
	out GEBL_CMOS_OUTPUT, al
	call iowait

	mov al, dh
	out GEBL_CMOS_OVERWRITE, al	; overwrite with better value
	call iowait

; prepare the mid mem loop
	mov cx, ((1<<20)*14)/0x1000	; max blocks untill hole
	mov esi, GEBL_EXT_BASE
	or esi, 0xffc	; last word of block

.midmem:
	memtest esi
	jc .midend
	add esi, GEBL_PROBE_BLOCKSIZE
	dec cx
	jz .midend	; we arrived at mem hole if the zf is set

	jmp .midmem

.midend:
	mov edx, ~0xffc
	and esi, edx
	sub esi, GEBL_EXT_BASE

%ifdef __DEBUG
	push dword 0
	push dword 0
	push dword 16
	push esi
	call printnum
	add esp, 4*4
%endif

	; get it in cmos now..

.highmem:

.done:
	pop ax		; re-set the old pic masks
	out GEBL_PIC2_DATA, al
	pop ax
	out GEBL_PIC1_DATA, al

	pop ebp
	ret