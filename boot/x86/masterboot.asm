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

%include "boot/x86/include/masterboot.h"

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
	mov sp, GEBL_LOADOFF
	mov bp, sp
	sti
main:
; 	es is already set to 0
	mov di, GEBL_BUFOFF
	mov si, _start ; beginning of the source
	push si
	mov cx, 512/2
	; we will move 2 bytes (words) at ones
	cld
	rep movsw
	
	xor dh, dh
	push dx ; drive number

	jmp GEBL_BUFSEG:GEBL_JUMPOFF
;
; GEBL_JUMPOFF gets us to the offset of migrate in the new seg:offset address. It is defined as: 
; (0x7c00 - addressof(migrate)) + 0x500 (0x500 = buffer offeset)
;

migrate:
; here we should use the partition table to indicate what offset we should use to load the first sector
; of the active (bootable) partition. Keep in mind that we moved our ass to here in a wicked way.

	xor ax, ax
	mov dx, [bp-4]
	int 0x13
%ifdef __FLOPPY
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
	mov bx, 0x7c0
	mov es, bx
	xor bx, bx
%elifdef __HDD
.chs:
	mov ax, 0x800
	mov dx, [bp-4]	; drive number
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
	mov si, GEBL_BUFOFF+GEBL_PART_TABLE
	mov ax, word [si+8]	; the pt
	div cx		; ax = temp value 	dx = sector (0-based)
	add dx, 1	; make sector 1-based
	push dx		; save the sector num for a while

	xor dx, dx	; clean modulo
	div bx		; ax = cylinder		dx = head

	mov ch, al	; low bits of the cylinder
	xor al, al
	shr ax, 2	; shift the 2 high bits of the cylinder into al
	pop bx		; get the sector
	or al, bl	; store sector in al together with high cyl
	mov cl, al

	shl dx, 8	; move dh, dl
	pop bx		; drive number
	mov dl, bl
	push bx		; store drive num again
	
	xor bx, bx	; segment 0
	mov es, bx
	mov bx, 0x7c00	; buffer
	mov ax, 0x201
	int 0x13
	jnc .checkboot

.lba:
	mov ah, 0x41
	mov bx, 0x55aa

	pop si		; ptable off first
	pop dx		; then the drive number
	push dx		; push everyting back up again
	push si

	int 0x13
	jc .chs

	mov ah, 0x42
	pop dx
	push dx

	mov si, GEBL_BUFOFF+GEBL_PART_TABLE
	push si
	mov cx, word [si+8]
	mov bx, word [si+10]
	mov si, dap
 	mov [si+8], cx
	mov [si+10], bx


%else
; nothing is defined about FDD's and HDD's, could be usb. Use CHS to be sure.
%endif
; 	issue an bios interrupt to read the sectors from the disk as defined above depending 
; 	on how it is compiled
.rdeval:
	int 0x13
	jc .error2

%ifdef __HDD
.checkboot:
	test byte [si], 0x80
	jz .error3
%endif

	mov al, 0x47
	call print
	jmp end

.error:
	mov al, 0x42
	call print
	jmp $

.error2:
	mov al, 0x43
	call print
	jmp $

.error3:
	mov al, 0x44
	call print
	jmp $

.error4:
	mov al, 0x45
	call print
	jmp $

.error5:
	mov al, 0x46
	call print
	jmp $

end:
	pop dx
	ret
	;jmp GEBL_LOADSEG:GEBL_LOADOFF
	cli
	hlt
	jmp end

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

times 446 - ($-$$) db 0
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

times 510 - ($-$$) db 0
dw 0xaa55