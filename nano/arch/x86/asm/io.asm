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

[GLOBAL outb]
[GLOBAL outw]
[GLOBAL inb]
[GLOBAL ioWait]

outb: ; outb (char data, short port)
  enter
  
  xor eax, eax
  xor edx, edx
  
  mov eax, [ebp+12]
  mov edx, [ebp+8]
  
  out dx, al
  
  return

outw: ; outw (short data, short port)  I'm a c programmer, not asm, so I don't know for sure if this works correct.
  enter
  
  xor eax, eax
  xor edx, edx
  
  mov eax, [ebp+16]
  mov edx, [ebp+8]
  
  out dx, al
  
  return

inb: ; inb (short port)
  enter
  
  xor eax, eax
  mov edx, [ebp+8]
  
  in al, dx
  
  return
  
ioWait: ; ioWait(void)
  xor eax, eax
  out 0x80, al
  ret
  
[GLOBAL readCR0]
readCR0:
  mov eax, cr0
  ret

[GLOBAL writeCR0]
  enter
  
  mov eax, [ebp+8]
  mov cr0, eax
  
  return
  
[GLOBAL readCR3]
readCR3:
  mov eax, cr3
  ret
  
[GLOBAL writeCR3]
writeCR3:
  enter
  
  mov eax, [ebp+8]
  mov cr3, eax
  
  return
  
[GLOBAL mutexEnter]
mutexEnter:
  enter

  mov eax, 1
.spin:
  xchg [ebp+8], eax
  test eax, eax
  jnz .spin
  
  return

[GLOBAL mutexRelease]
mutexRelease:
  enter
  xor eax, eax
  mov [ebp+8], eax
  return
