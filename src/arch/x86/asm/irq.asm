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
%include "asm/isr.mac"

; Define the irq headers
irq 0
irq 1
irq 2
irq 3
irq 4
irq 5
irq 6
irq 7
irq 8
irq 9
irq 10
irq 11
irq 12
irq 13
irq 14
irq 15

[EXTERN irqHandle]

; All ir1 headers call this stub.
irqStub:
  pusha ; Push all registers
  
  mov eax, 0x10 ; Set the data segments
  mov dx, ds
  push edx
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  
  call irqHandle ; Call the C function
  
  pop edx ; Reset the data segments
  mov ds, dx
  mov es, dx
  mov fs, dx
  mov gs, dx
  
  popa ; Pop all registers
  
  add esp, 8 ; Fix the stack
  sti
  iret ; Return