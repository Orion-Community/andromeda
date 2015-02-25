;   Orion OS, The educational operatingsystem
;   Copyright (C) 2011 - 2015  Bart Kuivenhoven

;   This program is free software: you can redistribute it and/or modify
;   it under the terms of the GNU General Public License as published by
;   the Free Software Foundation, either version 3 of the License, or
;   (at your option) any later version.

;   This program is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;   GNU General Public License for more details.

;   You should have received a copy of the GNU General Public License
;   along with this program.  If not, see <http://www.gnu.org/licenses/>.

; The assembly headers for the interrupts.
[GLOBAL halt]
[GLOBAL endProg]
[GLOBAL x86_eflags_test]
[GLOBAL x86_get_vendor]
[GLOBAL getCS]
[GLOBAL getDS]
[GLOBAL getSS]
[GLOBAL getESP]
[GLOBAL getCR2]
[GLOBAL getCR3]
[GLOBAL setCR3]
[GLOBAL setPGBit]
[GLOBAL pgbit]
[GLOBAL intdbg]
[GLOBAL elfJump]
[EXTERN mutex_lock]
[EXTERN mutex_unlock]

[GLOBAL disableInterrupts]

mutex   dd	0 ;The mutex variable
pgbit	dd	0 ;Paging is disabled per default
		  ;Booleans have been typedefed as unsigned char

%include "asm/call.mac"

disableInterrupts:
	pushfd		; Push flags to stack
	cli		; Disable interrupts
	pop eax		; Fetch the flags
	and eax, 1 << 9 ; Check if interrupts were enabled
	shr eax, 0
	jz .false
	mov eax, 1	; If previously enabled, return 1
	jmp .return
.false:
	xor eax, eax	; If not previously enabled, return 0
.return:
	ret		; Return

halt:
        pushfd		; Make sure interrupt flag is saved
        sti		; Enable interrupts
        hlt		; Make the cpu wait for interrupts
        popfd		; Restore interrupt flag to whatever it was before
        ret		; Return

; Prototype in include/arch/x86/cpu.h
x86_eflags_test:
        enter                   ; set up call frame

        push ebx
        mov ebx, [ebp+8]        ; Get the parameters

        pushfd                  ; Store the eflags on stack
        pushfd                  ; Get them again, but this time to edit
        xor dword [esp], ebx          ; Set the bit(s) to test
        popfd                   ; Write flags to register

        pushfd                  ; Read the flags again
        pop eax                 ; Put the flags into the return register
        xor eax, [esp]          ; Filter out the changes

        popfd                   ; Restore the original flags
        pop ebx

        return                  ; Restore the call frame and return


; Prototype in include/arch/x86/cpu.h
x86_get_vendor:
        enter
        push ebx
        push ecx
        push edx
        xor eax, eax
        cpuid

        cmp ebx, "Genu"
        jz .intel
        cmp ebx, "Auth"
        jz .amdTest
        cmp ebx, "AMDi"
        jz .amdTest

        xor eax, eax
        jmp .return;

.intel:
        cmp ecx, "ntel"
        jnz .unknown
        mov eax, 1
        jmp .return

.unknown:
        xor eax, eax
        jmp .return

.amdTest:
        mov eax, 2

.return:
        pop edx
        pop ecx
        pop ebx
        return

getCS:
        xor eax, eax
        mov ax, cs
        ret

getDS:
        xor eax, eax
        mov ax, ds
        ret

getSS:
        xor eax, eax
        mov ax, ss
        ret

getESP:
        xor eax, eax
        mov eax, esp
        ret

getCR2:
        %ifdef X86
        mov eax, cr2
        %else
        mov rax, cr3
        %endif
        ret

getCR3:
        %ifdef X86
        mov eax, cr3
        %else
        mov rax, cr3
        %endif
        ret

msg db "NOT YET IMPLEMEMENTED!", 0

setCR3:
        enter
        pusha
        mov eax, mutex
        push eax
        call mutex_lock
        add esp, 4
%ifdef X86
        mov eax, [ebp+8]
        mov cr3, eax
%else
        mov rdi, msg
        call panic
%endif
        mov eax, mutex
        push eax
        call mutex_unlock
        add esp, 4
        popa
        return

setPGBit:
        push eax
        mov eax, cr0
        or eax, 0x80000000
        mov cr0, eax
        pop eax
        ret

intdbg:
        int3
        ret

endProg:
        cli
        hlt
        jmp endProg

