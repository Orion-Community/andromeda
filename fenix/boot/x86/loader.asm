;
;    Golden Eagle bootloader. Loads the fenix kernel.
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

;    You should have received a copy of the GNU General Public License
;    along with this program.  If not, see <http://www.gnu.org/licenses/>.
;

[BITS 16]
[ORG 0X7C00]

; EXTERN kernelmain

main:
	mov si, hello
	call println

	call enable_A20
	call check_a20

; setup GDT, A20 line

	or ax,ax
	jz panic ; ax = 0 means no A20

succes:
	mov si, loaded
	call println
	jmp $
	
; switch to protected mode and 32 bits
;	call kernelmain

panic:
	mov si, error
	call println
	jmp $

;
;  Output routines
; 

println:
	xor cx, cx ; strlen
	xor dx, dx

putchar:
	lodsb
	or al, al
	jz newline 			; 0 byte found
	mov ah, 0x0E 			; teletype output
	xor bh, bh			; page 0
	int 0x10
	inc cx
	jmp putchar

newline:
	xor bh, bh
	mov al, 0x0A
	mov ah, 0x0E
	int 0x10

endprintln:
	xor bh, bh
	mov al, 0x08 ; backspace
	inc dx
	int 0x10
	cmp cx, dx
	jne endprintln ; not at beginning yet

	retn

;
; enable A20 line
;

enable_A20:
        cli
 
        call    a20wait
        mov     al,0xAD
        out     0x64,al
 
        call    a20wait
        mov     al,0xD0
        out     0x64,al
 
        call    a20wait2
        in      al,0x60
        push    eax
 
        call    a20wait
        mov     al,0xD1
        out     0x64,al
 
        call    a20wait
        pop     eax
        or      al,2
        out     0x60,al
 
        call    a20wait
        mov     al,0xAE
        out     0x64,al
 
        call    a20wait
        sti
        ret
 
a20wait:
        in      al,0x64
        test    al,2
        jnz     a20wait
        ret
 
 
a20wait2:
        in      al,0x64
        test    al,1
        jz      a20wait2
        ret

;
; check_A20
;

check_a20:
    pushf
    push ds
    push es
    push di
    push si
 
    cli
 
    xor ax, ax ; ax = 0
    mov es, ax
 
    not ax ; ax = 0xFFFF
    mov ds, ax
 
    mov di, 0x0500
    mov si, 0x0510
 
    mov al, byte [es:di]
    push ax
 
    mov al, byte [ds:si]
    push ax
 
    mov byte [es:di], 0x00
    mov byte [ds:si], 0xFF
 
    cmp byte [es:di], 0xFF
 
    pop ax
    mov byte [ds:si], al
 
    pop ax
    mov byte [es:di], al
 
    mov ax, 0
    je check_a20__exit
 
    mov ax, 1
 
check_a20__exit:
    pop si
    pop di
    pop es
    pop ds
    popf
    sti
    ret

;
; Some sort of data segment
;

	hello db 'Loading Golden Eagle bootloader', 0x0
	loaded db 'Golden Eagle loaded - Calling Fenix kernel', 0x0
	error db 'Failed to load Golden Eagle succesfull...', 0x0
;
; End
;

times 512-($-$$)-2 db 0
dw 0xAA55

