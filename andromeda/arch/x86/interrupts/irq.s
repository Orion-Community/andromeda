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

irqStub:
  pusha
  
  mov eax, 0x10
  mov dx, ds
  push edx
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  
  call irqHandle
  
  pop edx
  mov ds, dx
  mov es, dx
  mov fs, dx
  mov gs, dx
  
  popa
  
  add esp, 8
  
  iret