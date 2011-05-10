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
[ORG 0x7C00]
jmp 0x0:main
; nop
; 	bpbBytesPerSector:  	DW 512
; 	bpbSectorsPerCluster: 	DB 1
; 	bpbReservedSectors: 	DW 1
; 	bpbNumberOfFATs: 	DB 2
; 	bpbRootEntries: 	DW 224
; 	bpbTotalSectors: 	DW 2880
; 	bpbMedia: 	        DB 0xF8
; 	bpbSectorsPerFAT: 	DW 9
; 	bpbSectorsPerTrack: 	DW 18
; 	bpbHeadsPerCylinder: 	DW 2
; 	bpbHiddenSectors: 	DD 0
; 	bpbTotalSectorsBig:     DD 0
; 	bsDriveNumber: 	        DB 0x81
; 	bsUnused: 	        DB 0
; 	bsExtBootSignature: 	DB 0x29
; 	bsSerialNumber:	        DD 0xa0a1a2a3
; 	bsVolumeLabel: 	        DB "MOS FLOPPY "
; 	bsFileSystem: 	        DB "FAT12   "

main: ; entry point
	cli
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov sp, 0x7BFE
	sti

	mov byte [bootdisk], dl
	call loadimage
	shr ax, 8
	or al, al
	jz .loaded

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

	mov dl, byte [bootdisk]
	jmp 0x0:0x7E00
	mov si, failed
	call println
	cli
	jmp $

;
; Image loader
;

%include 'boot/x86/stage1/loadimage.asm'

;
; Print routines
;

%include 'boot/x86/println.asm'

;
; Since flat binary is one big heap of code without sections, is the code below some sort of data section.
;

	booted db 'GEBL has been loaded by the bios! Executing...', 0x0
	failed db '(0x0) Failed to load the next stage.. ready to reboot. Press any key.', 0x0

times 510 - ($ - $$) db 0
dw 0xAA55