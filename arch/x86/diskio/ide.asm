;
;    This piece of code is dedicated to the ide driver -> reading/writing to the disk.
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

%include "arch/x86/diskio/include/ide.asmh"

[SECTION .text]

[EXTERN printnum]

[GLOBAL ide_init]
ide_init:
	push ebp
	mov ebp, esp

	mov edi, [ebp+8]
	mov esi, 0x7c00
	cld
	xor edx, edx	; max 4 partitions

.looptop:
	test byte [esi], 0x80
	jz .continue
	mov eax, edx

.continue:
	mov ecx, 0x8
	rep movsw
.loopend:
	inc edx
	test edx, 0x4
	jz .looptop
.end:
	pop ebp
	ret

[GLOBAL ide_read]
ide_read: ; ide_read(sectors to read, dest buffer, ptable pointer, relative lba)
	push ebp
	mov ebp, esp

	mov eax, [ebp+8]
	test eax, eax
	jz .fail	; you should read more then zero sectors - idiot
	test eax, 0x400000	; are you trying to read more than 2gb? - idiot
	jnz .fail

	mov ebx, [ebp+16]
	test byte [ebx], 0x80
	jz .fail2

	cmp dword [ebx+OL_PTABLE_TOT_SECTORS], eax
	jb .fail2

	mov dx, OL_MASTER_ATA_STATUS
	in al, dx
	test al, 0xa8
	jne .fail2

	mov edx, [ebp+20]	; relative lba
	cmp edx, 0xfffffff
	jg short .test

	add eax, edx	; sectors to read + relative lba
	add eax, dword [ebx+OL_PTABLE_LBA]
	cmp eax, 0xfffffff

.test:
	mov dx, OL_MASTER_ATA_BASE
	jle .lba28

.lba48:
	; implement 48 bit lba

.lba28:
	; implement 28 bit lba

.end:
	clc
	pop ebp
	ret

.fail:
	stc
	ret

.fail2:
	jmp $