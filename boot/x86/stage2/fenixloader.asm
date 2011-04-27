;
;    Called by the first stage bootloader. This is the second stage and loads the kernel.
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

[bits 16]
;[org 0x1000]

main2:
	mov si, exec
	call print
	
	call enable_A20
	xor ax, 0001b
	jnz .a20success

.a20fail:
	mov si, a20failed
	call print
	jmp .bailout

.a20success:
	mov si, a20ok
	call print
	
	cli
	xor ax, ax
	mov ds, ax
	lgdt [gdtr]

	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov ss, ax

	mov eax, cr0
	or eax, 00000001b
	mov cr0, eax
	jmp 0x8:do_pm
	;jmp 0x8:.bailout

.bailout:
	mov si, reboot
	call print
	xor ah, ah
	int 0x16 ; wait for char
	int 0x19 ; this bios loads sector 1 into 0x0:0x7C00 and executes (= reboot)
	
	cli
	jmp $

;
; Output routines
;

;%include 'print.asm'

;
; A20 line
;

%include 'stage2/A20.asm'

[BITS 32]
do_pm:
	mov si, reboot
	call print
	xor ah, ah
	int 0x16 ; wait for char
	int 0x19 ; this bios loads sector 1 into 0x0:0x7C00 and executes (= reboot)
	jmp $

;
; Data section
;

	exec db 'The second stage bootloader has been called and is executing..', 0x0
	a20ok db 'A20 Line is successfully enabled!', 0x0
	a20failed db 'Setting up the A20 line failed, ready to reboot...', 0x0
	reboot db 'Press a key to reboot', 0x0

gdt:
    times 8 db 0
    gdt_code:               ; Code segment, read/execute, nonconforming
        dw 0FFFFh
        dw 0
        db 0
        db 0x9A
        db 0xCF
        db 0
    gdt_data:               ; Data segment, read/write, expand down
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

times 1022 - ($ - $$) db 0
last:
	dw 0x8899