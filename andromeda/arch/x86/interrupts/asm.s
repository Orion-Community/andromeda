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
[GLOBAL DetectAPIC]
[GLOBAL getVendor]
[GLOBAL getCS]
[GLOBAL getDS]
[GLOBAL getSS]
[GLOBAL getESP]
[GLOBAL getCR3]
[GLOBAL setCR3]
[GLOBAL toglePEbit]
[GLOBAL pgbit]

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
  pushfd ; Store the flags register
  pop eax ; pop it into eax
  sti ; Start all interrupts
  hlt ; Stop the CPU untill another interrupt occurs.
  test eax, 0x200 ; Test for the interrupt bit
  jnz .resetInts ; if enabled stop interrupts again.
  ret
.resetInts:
  cli ; No interrupts allowed from this point
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
  
getCR3:
  %ifdef X86
  mov eax, cr3
  %else
  mov rax, cr3 ; Hope this returns CR3
  %endif
  ret


setCR3:
  enter
  %ifdef X86
  mov eax, [ebp+8]
  mov cr3, eax
  %else
  mov rax, [ebp+16] ; Hope this gets the right value
  mov cr3, rax
  %endif
  return
  
toglePGbit:
  mov eax, cr0
  xor eax, 0x80000000
  mov cr0, eax
  mov eax, [pgbit]
  not eax
  mov [pgbit], eax
  ret