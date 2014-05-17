;   Orion OS, The educational operatingsystem
;   Copyright (C) 2011  Bart Kuivenhoven

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
[GLOBAL DetectAPIC]
[GLOBAL getVendor]
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
;[GLOBAL enableInterrupts]

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
	jmp .return
	jz .false
	mov eax, 1	; If previously enabled, return 1
	jmp .return
.false:
	xor eax, eax	; If not previously enabled, return 0
.return:
	ret		; Return

;enableInterrupts:
;	sti		; Enable interrupts
;	xor eax, eax 	; Set return value to 0, to signal no error has occured
;	ret		; Return

halt:
        pushfd		; Make sure interrupt flag is saved
        sti		; Enable interrupts
        hlt		; Make the cpu wait for interrupts
        popfd		; Restore interrupt flag to whatever it was before
        ret		; Return

DetectAPIC:
        enter

        push eax
        push ebx
        push edx

        call getVendor
        cmp eax, 1
        jnz .testAPIC
        cmp eax, 2
        jnz .testAPIC
        jmp .err

.testAPIC:
        mov eax, 1 ; prepare CPUID
        cpuid ; Issue CPUID

        and edx, 1<<9 ; mask the flag out
        mov eax, edx ; return value to eax

        pop edx
        pop ebx
        pop eax

        return

.err: ; invalid CPUID
        pop edx
        pop ebx
        pop eax

        xor eax, eax ; return -1
        sub eax, 1

        return

getVendor:
        enter
        push ebx
        push ecx
        push edx
        xor eax, eax
        cpuid

        cmp ebx, "Genu"
        jz .intel
        cmp ebx, 0x68747541
        jz .amdTest
        xor eax, eax
        pop edx
        pop ecx
        pop ebx
        return

.intel:
        mov eax, 1
        pop edx
        pop ecx
        pop ebx
        return

.amdTest:
        pop edx
        pop ecx
        pop ebx
        mov eax, 2
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

elfJump:
        mov ebx, [esp+12] ; Give the modules in ebx
        mov eax, [esp+8] ; Give the memory map in eax
        jmp [esp+4] ; jump toward the argument, don't care about the stack
