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

	call enableA20
;	call check_a20

; setup GDT, A20 line

	or eax, eax
	jz success ; ax = 0 means no A20

success:
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

enableA20:
	;; This subroutine will enable the A20 address line in the keyboard
	;; controller.  Takes no arguments.  Returns 0 in EAX on success, 
	;; -1 on failure.  Written for use in 16-bit code, see lines marked
	;; with 32-BIT for use in 32-bit code.

	pusha

	;; Make sure interrupts are disabled
	cli

	;; Keep a counter so that we can make up to 5 attempts to turn
	;; on A20 if necessary
	mov CX, 5

	.startAttempt1:		
	;; Wait for the controller to be ready for a command
	.commandWait1:
	xor AX, AX
	in AL, 64h
	bt AX, 1
	jc .commandWait1

	;; Tell the controller we want to read the current status.
	;; Send the command D0h: read output port.
	mov AL, 0D0h
	out 64h, AL

	;; Wait for the controller to be ready with a byte of data
	.dataWait1:
	xor AX, AX
	in AL, 64h
	bt AX, 0
	jnc .dataWait1

	;; Read the current port status from port 60h
	xor AX, AX
	in AL, 60h 

	;; Save the current value of (E)AX
	push AX			; 16-BIT
	;; push EAX		; 32-BIT

	;; Wait for the controller to be ready for a command
	.commandWait2:
	in AL, 64h
	bt AX, 1
	jc .commandWait2

	;; Tell the controller we want to write the status byte again
	mov AL, 0D1h
	out 64h, AL	

	;; Wait for the controller to be ready for the data
	.commandWait3:
	xor AX, AX
	in AL, 64h
	bt AX, 1
	jc .commandWait3

	;; Write the new value to port 60h.  Remember we saved the old
	;; value on the stack
	pop AX			; 16-BIT
	;; pop EAX		; 32-BIT

	;; Turn on the A20 enable bit
	or AL, 00000010b
	out 60h, AL

	;; Finally, we will attempt to read back the A20 status
	;; to ensure it was enabled.

	;; Wait for the controller to be ready for a command
	.commandWait4:
	xor AX, AX
	in AL, 64h
	bt AX, 1
	jc .commandWait4

	;; Send the command D0h: read output port.
	mov AL, 0D0h
	out 64h, AL	

	;; Wait for the controller to be ready with a byte of data
	.dataWait2:
	xor AX, AX
	in AL, 64h
	bt AX, 0
	jnc .dataWait2

	;; Read the current port status from port 60h
	xor AX, AX
	in AL, 60h

	;; Is A20 enabled?
	bt AX, 1

	;; Check the result.  If carry is on, A20 is on.
	jc .success

	;; Should we retry the operation?  If the counter value in ECX
	;; has not reached zero, we will retry
	loop .startAttempt1


	;; Well, our initial attempt to set A20 has failed.  Now we will
	;; try a backup method (which is supposedly not supported on many
	;; chipsets, but which seems to be the only method that works on
	;; other chipsets).


	;; Keep a counter so that we can make up to 5 attempts to turn
	;; on A20 if necessary
	mov CX, 5

	.startAttempt2:
	;; Wait for the keyboard to be ready for another command
	.commandWait6:
	xor AX, AX
	in AL, 64h
	bt AX, 1
	jc .commandWait6

	;; Tell the controller we want to turn on A20
	mov AL, 0DFh
	out 64h, AL

	;; Again, we will attempt to read back the A20 status
	;; to ensure it was enabled.

	;; Wait for the controller to be ready for a command
	.commandWait7:
	xor AX, AX
	in AL, 64h
	bt AX, 1
	jc .commandWait7

	;; Send the command D0h: read output port.
	mov AL, 0D0h
	out 64h, AL	

	;; Wait for the controller to be ready with a byte of data
	.dataWait3:
	xor AX, AX
	in AL, 64h
	bt AX, 0
	jnc .dataWait3

	;; Read the current port status from port 60h
	xor AX, AX
	in AL, 60h

	;; Is A20 enabled?
	bt AX, 1

	;; Check the result.  If carry is on, A20 is on, but we might warn
	;; that we had to use this alternate method
	jc .warn

	;; Should we retry the operation?  If the counter value in ECX
	;; has not reached zero, we will retry
	loop .startAttempt2


	;; OK, we weren't able to set the A20 address line.  Do you want
	;; to put an error message here?
	jmp .fail


	.warn:
	;; Here you may or may not want to print a warning message about
	;; the fact that we had to use the nonstandard alternate enabling
	;; method

	.success:
	sti
	popa
	xor EAX, EAX
	ret

	.fail:
	sti
	popa
	mov EAX, -1
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

