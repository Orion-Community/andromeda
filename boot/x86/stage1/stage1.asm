;
;    Golden Eagle Boot loader. The first stage is loaded by the bios at 0x0:0x7C00.
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
[ORG 0x7c00]

jmp short main
nop

main: ; entry point
	jmp 0x0:.flush
.flush:
	cli
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, 0x7c00
	mov bp, sp
	sti

	mov [bootdisk], dl
	push si
	push dx

	test byte [si], 0x80
	jz .bailout
.reset:
	xor ax, ax
	int 0x13
	jc .reset

.chs:
	mov ax, 0x800
	mov dx, [bp-4]	; drive number
	xor di, di	; work around for some buggy bioses
	mov es, di
	int 0x13
	jc .bailout

	and cl, 00111111b	; max sector number is 0x3f
	inc dh		; make head 1-based
	xor bx, bx
	mov bl, dh	; store head

	xor dx, dx	; clean modulo storage place
	xor ch, ch	; get all the crap out of ch
	mov ax, word [si+8]	; the pt
	div cx		; ax = temp value 	dx = sector (0-based)
	add dx, 2	; make sector 1-based and read second sector
	push dx		; save the sector num for a while

	xor dx, dx	; clean modulo
	div bx		; ax = cylinder		dx = head

	mov ch, al	; low bits of the cylinder
	xor al, al
	shr ax, 2	; [6 ... 7] high bits of the cylinder
	pop bx		; get the sector
	or al, bl	; [0 ... 5] bits of the sector number
	mov cl, al

	shl dx, 8	; move dh, dl
	pop bx		; drive number
	mov dl, bl
	push bx		; store drive num again
	
	xor bx, bx	; segment 0
	mov es, bx
	mov bx, 0x7e00	; buffer
	mov ax, 0x201
	int 0x13
	jc .lba
	jmp .loaded

.lba:
	mov cx, word [si+8]
	mov dx, word [si+10]
	inc cx
	mov si, dap
	mov [si+8], cx
	mov [si+10], dx

	mov ah, 0x41
	mov bx, 0x55aa
	pop dx
	push dx
	int 0x13
	jc .bailout

	mov ah, 0x42
	pop dx
	push dx
	int 0x13
	jc .bailout

	jmp .loaded
.bailout:
	mov si, failed
	call println
	xor ax, ax
	int 0x16
	int 0x19
	cli
	jmp $

.loaded:
	mov si, booted
	call println

	pop dx
	pop si
	jmp 0x0:0x7E00
	cli
	jmp $

;
; Image loader
;

;%include 'boot/x86/stage1/loadimage.asm'

;
; Print routines
;

%include 'boot/x86/println.asm'

;
; Since flat binary is one big heap of code without sections, is the code below some sort of data section.
;

dap:
	db 0x10      	; register size
	db 0      	; reserved, must be 0
	dw 0x2      	; sectors to read
	dw 0x7e00   	; memory offset
	dw 0x0   	; memory segment
	dq 0x0		; starting sector (sector to read, s1 = 0)

	bootdisk db 0x0
	booted db 'EBL has been loaded by the bios! Executing...', 0x0
	failed db '(0x0) Failed to load the next stage.. ready to reboot. Press any key.', 0x0

times 510 - ($ - $$) db 0
dw 0xAA55