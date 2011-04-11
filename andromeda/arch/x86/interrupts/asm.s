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
[GLOBAL DetectAPIC]
[GLOBAL getVendor]
[GLOBAL getCS]
[GLOBAL getDS]
[GLOBAL getSS]
[GLOBAL getESP]

%include "asm/call.mac"

sti: ; Start interrupts from C level code
  sti ; Start interrupts
  ret
  
cli: ; Shut down interrupts from C level code
  cli ; Stop interrupts
  ret
  
DetectAPIC:
  enter
  
  call getVendor
  cmp eax, 1
  jnz test
  cmp eax, 2
  jnz test
  jmp err
  
test:
  mov eax, 1 ; prepare CPUID
  cpuid ; Issue CPUID
  
  xor edx, 0x100 ; mask the flag out
  mov eax, edx ; return value to eax
  
  return
    
err: ; invalid CPUID
  xor eax, eax ; return -1
  sub eax, 1
  return
  
getVendor:
  enter
  xor eax, eax
  cpuid
  
  cmp ebx, "Genu"
  jz intel
  cmp ebx, 0x68747541
  jz amdTest
  xor eax, eax
  return
  
intel:
  mov eax, 1
  return
  
amdTest:
  mov eax, 2
  return

getCS:
  enter
  xor eax, eax
  mov ax, cs
  return
  
getDS:
  enter
  xor eax, eax
  mov ax, ds
  return

getSS:
  enter
  xor eax, eax
  mov ax, ss
  return

getESP:
  enter
  xor eax, eax
  mov eax, esp
  return