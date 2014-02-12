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

%include "asm/call.mac"

[GLOBAL arch_syscall]
arch_syscall:
        enter
        mov eax, [ebp+0x08]
        mov ebx, [ebp+0x0C]
        mov ecx, [ebp+0x10]
        mov edx, [ebp+0x14]

        int 0x80

        return

[GLOBAL readCR0]
readCR0:
        mov eax, cr0
        ret

[GLOBAL writeCR0]
        enter

        push eax

        mov eax, [ebp+8]
        mov cr0, eax

        pop eax

        return

[GLOBAL readCR3]
readCR3:
        mov eax, cr3
        ret

[GLOBAL writeCR3]
writeCR3:
        enter

        push eax

        mov eax, [ebp+8]
        mov cr3, eax

        pop eax

        return

[GLOBAL mutex_lock]
mutex_lock:
        enter

        push eax
        push ebx

        mov eax, 1
        mov ebx, [ebp+8]
.spin:
	mfence
        xchg [ebx], eax
	mfence
        test eax, eax
        jnz .spin

        pop ebx
        pop eax

        return

[GLOBAL mutex_test] ; // Return 0 if mutex was unlocked, 1 if locked
mutex_test:
        enter

        push ebx

        mov eax, 1
        mov ebx, [ebp+8]
	mfence
        xchg [ebx], eax
	mfence

        pop ebx

        return


[GLOBAL mutex_unlock]
mutex_unlock:
        enter

        push eax
        push ebx

        xor eax, eax
        mov ebx, [ebp+8]
	mfence
        mov [ebx], eax

        pop ebx
        pop eax

        return

