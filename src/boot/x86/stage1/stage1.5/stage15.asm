;
;    Entry point for stage 1.5. This sector will use a dynamic sector loader to load the second stage and the micro kernel.
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
[ORG 0x7e00]

%include "boot/masterboot.asmh"
%include "boot/stage15.asmh"

jmp short main
nop

gdt:
    times 8 db 0
    UNREAL_SEG equ $ - gdt	; Data segment, read/write, expand down
        dw 0FFFFh
        dw 0
        db 0
        db 0x92
        db 0xCF
        db 0
gdt_end equ $ - gdt

gdtr:
	dw gdt_end - 1; gdt limit = size
	dd gdt ; gdt base address

[GLOBAL main]
main:
	mov di, OL_PTABLE_PTR
	mov si, OL_BUFOFF+OL_PART_TABLE
	push di
	mov cx, 0x40
	cld
	rep movsw
	push dx

	call openA20
	jc .bailout

	jmp .unrealmode

.bailout:
	mov si, failed
	call println
	cli
	jmp $

.unrealmode:
; before we load the core image and the second stage with the bios we
; have to hack the processor into unreal mode.
	cli
	push ds	; save for later
	lgdt [gdtr]
	mov eax, cr0
	or eax, 1b
	mov cr0, eax

	mov ax, UNREAL_SEG
	mov ds, ax

	mov eax, cr0
	xor eax, 1b
	mov cr0, eax
	pop ds
	sti
; hello unreal mode

.loadcore:
	call calcsectors

	sub eax, OL_SECTOR_BLOCK ; read x-sectors each loop
	push eax
	xor ebp, ebp ; ebp should not be incremented at first loop
	jmp .cpylooptop		; calcsectors did alread read 4 sectors
				; to the buffer.

.looptop:
	pop eax
	cmp eax, OL_SECTOR_BLOCK
	jb .lastsectors		; when there are less then x sectors to read

	sub ax, OL_SECTOR_BLOCK	; prevent endless loop
	push eax
	mov ax, OL_SECTOR_BLOCK
	
	add ebp, OL_SECTOR_BLOCK	; ebp holds the amount of read sectors (minus first 4)
	jmp .loadsector

.lastsectors:
	dec eax		; read last sectors one by one
	js .end
	push eax
	inc ebp
	mov ax, 1

.loadsector:
	mov cx, ax
	mov eax, dword [OL_PTABLE_PTR+8]
	add eax, 5	; sixth sector offset
	add eax, ebp	; make sure we don't read the same sector every time
	xor ebx, ebx
	mov es, bx
	mov di, OL_SECTOR_BUFFER

	call int13read
	jc .bailout

; now we will copy the sector from the buffer to its final destination
.cpylooptop:
	and ecx, 0xffff
	shl cx, 9	; cx *= 512
	shl ebp, 9	; ebp *= 512
	mov edi, OL_DESTINATION_BUFFER	; start of destination
	mov esi, OL_SECTOR_BUFFER	; buffer address
	add edi, ebp	; adjust destination for current read
	add ebx, ebp
	shr ebp, 9	; revert back

.cpysectors:	; actualy a memcpy
	mov al, byte [ds:esi]
	mov byte [ds:edi], al

	add edi, 1	; copy 4 bytes every memcpy loop
	add esi, 1

	sub cx, 1	; we do NOT want a never ending loop
	jnz .cpysectors

	jmp .looptop

.end:
; 	mov edi, 0x1100000
; 	mov ax, word [endptr]
; 	mov [ds:edi], ax
	pop dx
	pop si

	jmp 0x0:0x8200

	jmp .bailout

;
; 16-bit bios disk interface
;

%include 'disk.asm'

;
; Print routine
;

%include 'println.asm'

; 
; Routine to open the A20-gate
; 

%include 'a20.asm'
	
	failed db '0x2', 0x0

times 1024 - ($ - $$) db 0
