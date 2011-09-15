;
;    The masterboot record. Loads the first sector of the active partition.
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

%include "boot/masterboot.asmh"

[BITS 16]
[ORG 0x7c00]

; This code is located at the first sector of the harddisk. It is loaded by the bios to address 0x0:0x7c00.
; The code will migrate itself to address 0x50:0x0. From there it will load the first sector of the active partition
; to address 0x0:0x7c00.

_start:
jmp short start
nop
; The bios parameter block, in the case it is a floppy
%ifdef __FLOPPY
	times 8 db 0			; fill up 
	dw 512				; bytes per sector 
	db 1 				; sectors per cluster 
	dw 2 				; reserved sector count 
	db 2 				;number of FATs 
	dw 16*14 			; root directory entries 
	dw 18*2*80 			; 18 sector/track * 2 heads * 80 (density) 
	db 0F0h 			; media byte 
	dw 9 				; sectors per fat 
	dw 18 				; sectors per track 
	dw 2 				; number of heads 
	dd 0 				; hidden sector count 
	dd 0 				; number of sectors huge 
	bootdisk db 0 			; drive number 
	db 0 				; reserved 
	db 29h 				; signature 
	dd 0 				; volume ID 
	times 11 db 0	 		; volume label 
	db 'FAT12   '                 ; file system type
%endif

start:
	cli
; 	we are not safe here
	jmp 0x0:.flush
; 	there are buggy bioses which load you in at 0x7c0:0x0
.flush:
	xor ax, ax
	mov ds, ax 	; tiny memory model

	mov es, ax	; general and extra segmets
	mov gs, ax
	mov fs, ax

	mov ss, ax	; stack segment
	mov sp, OL_LOADOFF
	mov bp, sp
	sti
main:
; 	es is already set to 0
	mov di, OL_BUFOFF
	mov si, _start ; beginning of the source
	push si
	mov cx, 512/2
	; we will move 2 bytes (words) at ones
	cld
	rep movsw
	
	push dx ; drive number

	jmp OL_BUFSEG:OL_JUMPOFF
;
; OL_JUMPOFF gets us to the offset of migrate in the new seg:offset address. It is defined as: 
; (0x7c00 - addressof(migrate)) + 0x500 (0x500 = buffer offeset)
;

migrate:
; here we should use the partition table to indicate what offset we should use to load the first sector
; of the active (bootable) partition.
%ifdef __HDD
	mov si, OL_BUFOFF+OL_PART_TABLE
	xor cx, cx
toploop:
	test cx, 100b ; cmp cx, 0x4
	jnz .error

	test byte [si], 0x80	; if not zero, partition is active => boot
	jnz .start_read
	add si, 0x10
	add cx, 1
	jmp toploop
%endif

.start_read:
	xor ax, ax
	pop dx
	push dx
	int 0x13
%ifdef __HDD
.lba:
	mov ah, 0x41
	mov bx, 0x55aa

	pop dx		; then the drive number
	push dx		; push everyting back up again

	int 0x13
	jc .chs

	pop dx
	push dx

	push si
	mov ax, word [si+8]
	mov cx, word [si+10]

	mov si, dap
 	mov word [si+8], ax
	mov word [si+10], cx

	mov ax, 0x4200
	int 0x13
	pop si	; restore the partition table
	jnc .end

.chs:
	mov ax, 0x800
	pop dx
	push dx
	xor di, di	; work around for some buggy bioses
	mov es, di
	int 0x13
	jc .error

	and cl, 00111111b	; max sector number is 0x3f
	inc dh		; make head 1-based
	xor bx, bx
	mov bl, dh	; store head

	xor dx, dx	; clean modulo storage place
	xor ch, ch	; get all the crap out of ch
	mov ax, word [si+8]	; the pt
	div cx		; ax = temp value 	dx = sector (0-based)
	add dx, 1	; make sector 1-based
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
	mov bx, OL_LOADOFF	; buffer
	mov ax, 0x201
	int 0x13

%elifdef __FLOPPY
	; read 1 sector
	; int 13h function 0x2
	mov ax, 0x201
	
	; track 0 and read at sector 2
	xor ch, ch
	mov cl, 0x2

	; head 0 and the drive number
	xor dh, dh
	pop dx
	push dx
	xor bx, bx
	mov es, bx
	mov bx, 0x7c00
	int 0x13
	jc .error

%endif
	jmp .end

.error:
	mov al, 0x30
	call print
	jmp $

.end:
	pop dx
	ret
	;jmp OL_LOADSEG:OL_LOADOFF
	cli
	hlt
	jmp .end

; -- print routine
;   character is expected in al

print:
	mov ah, 0x0E
	xor bh, bh
	int 0x10
	ret

%ifdef __HDD
sectors dw 0
maxHead dw 0

dap:
	db 0x10      	; register size
	db 0      	; reserved, must be 0
	dw 0x4      	; sectors to read
	dw 0x7c00   	; memory offset
	dw 0x0   	; memory segment
	dq 0x0		; starting sector (sector to read, s1 = 0)
%endif

%ifdef __DEBUG
times 446 - ($-$$) db 0
; first partition table
	db 0x80
	db 0x0
	dw 0x21
	dw 0xbe83
	dw 0x3f0b
	db 0x0
	db 0x08
	dw 0x0
	dw 0xb800
	dw 0x3b
; second partition table
	db 0x00
	db 0x0
	dw 0x21
	dw 0xbe83
	dw 0x3f0b
	db 0x0
	db 0x08
	dw 0x0
	dw 0xb800
	dw 0x3b
%endif

times 510 - ($-$$) db 0
dw 0xaa55
