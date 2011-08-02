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

%macro newline 0
	push 0xa
	call putc
	add esp, 4
%endmacro

%macro delay 1
	mov dx, %1
	in al, dx
	in al, dx
	in al, dx
	in al, dx
%endmacro

%macro printhex 1
	push 0
	push 0
	push 16
	push dword %1
	call printnum
	add esp, 4*4
%endmacro

[SECTION .data]

%include "arch/x86/diskio/include/ide.asmh"

[SECTION .text]

[EXTERN printnum]
[EXTERN putc]
[EXTERN sleep]

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

[GLOBAL ata_identify]
ata_identify:
	push ebp
	mov ebp, esp

.reset:
	mov dx, ATA_COMMAND(OL_PRIMARY_BUS_BASE)
	mov al, 0x4
	out dx, al
	xor al, al
	out dx, al

	mov dx, ATA_FEATURES(OL_PRIMARY_BUS_BASE)
	mov al, 0
	out dx, al

.selectdrive:
	mov dx, ATA_DRIVE_SELECT(OL_PRIMARY_BUS_BASE)
	mov al, 0xa0 ; select master drive
	out dx, al
	delay ATA_DCR(OL_PRIMARY_BUS_BASE)

.setValues:
	xor al, al
	mov dx, ATA_SECTOR_COUNT(OL_PRIMARY_BUS_BASE)
	out dx, al
	
	inc dx ; inc dx -> dx = 0xf3
	out dx, al

	inc dx
	out dx, al

	inc dx
	out dx, al

.identify:
	mov al, 0xec ; identify command
	mov dx, ATA_COMMAND(OL_PRIMARY_BUS_BASE)
	out dx, al

	delay ATA_DCR(OL_PRIMARY_BUS_BASE)

	mov dx, ATA_DCR(OL_PRIMARY_BUS_BASE)
	xor eax, eax

	in al, dx
	printhex eax
	newline

.testValues:
	mov dx, ATA_MID_LBA(OL_PRIMARY_BUS_BASE)
	push dx
	in al, dx
	printhex eax
	newline

	pop dx
	inc dx
	in al, dx
	printhex eax
	newline

.done:
	and eax, 0xff
	pop ebp
	ret
