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
[GLOBAL sti]
[GLOBAL cli]
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
[GLOBAL toglePGbit]
[GLOBAL pgbit]
[EXTERN mutexEnter]
[EXTERN mutexRelease]

mutex   dd	0 ;The mutex variable
pgbit	db	0 ;Paging is disabled per default
		  ;Booleans have been typedefed as unsigned char

%include "asm/call.mac"

sti: ; Start interrupts from C level code
  sti ; Start interrupts
  ret
  
cli: ; Shut down interrupts from C level code
  cli ; Stop interrupts
  ret

halt:
  %ifndef __COMPRESSED
  pushfd
  sti
  hlt
  popfd
  %else
  cli
  hlt
  %endif
  ret
  
DetectAPIC:
  enter
  
  call getVendor
  cmp eax, 1
  jnz .testAPIC
  cmp eax, 2
  jnz .testAPIC
  jmp .err
  
.testAPIC:
  mov eax, 1 ; prepare CPUID
  cpuid ; Issue CPUID
  
  xor edx, 0x100 ; mask the flag out
  mov eax, edx ; return value to eax
  
  return
    
.err: ; invalid CPUID
  xor eax, eax ; return -1
  sub eax, 1
  return
  
getVendor:
  enter
  xor eax, eax
  cpuid
  
  cmp ebx, "Genu"
  jz .intel
  cmp ebx, 0x68747541
  jz .amdTest
  xor eax, eax
  return
  
.intel:
  mov eax, 1
  return
  
.amdTest:
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
  mov eax, [mutex]
  push eax
  call mutexEnter
  add esp, 4
  %ifdef X86
  mov eax, [ebp+8]
  mov cr3, eax
  %else
  mov rdi, msg
  call panic
  %endif
  mov eax, [mutex]
  push eax
  call mutexRelease
  add esp, 4
  return
  
toglePGbit:
  mov eax, [mutex]
  push eax
  call mutexEnter
  add esp, 4
  mov eax, cr0
  xor eax, 0x80000000
  mov cr0, eax
  mov eax, [pgbit]
  not eax
  mov [pgbit], eax
  mov eax, [mutex]
  push eax
  call mutexRelease
  add esp, 4
  ret
  
[GLOBAL intdbg]
intdbg:
  int3
  ret
  
endProg:
  cli
  hlt
  jmp endProg
