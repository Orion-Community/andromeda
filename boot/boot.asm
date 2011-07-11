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

CORE_MAGIC	equ	0xC0DEBABE	; Lolz

[BITS 32]

[SECTION .boot]
[GLOBAL mboot]
[EXTERN code]                   ; Start of the '.text' section.
[EXTERN bss]                    ; Start of the .bss section.
[EXTERN end]
mboot:
  dd CORE_MAGIC
  dd start
  dd end

[SECTION .text]
[GLOBAL start]
[EXTERN core]
[EXTERN stack]
start:
  mov ecx, stack		; Set the new stack frame
  add ecx, 0x10000		; Add the size of the stack to the pointer
  mov ebp, ecx
  mov esp, ecx			; Stack grows down in memory and we're at the
  push esp			; top of the minimum required memory
  cli
  call core
  jmp $