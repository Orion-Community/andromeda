;
;    The interrupt request header.
;    Copyright (C) 2011 Michel Megens
;
;    This program is free software: you can redistribute it and/or modify
;    it under the terms of the GNU General Public License as published by
;    the Free Software Foundation, either version 3 of the License, or
;    (at your option) any later version.
;
;    This program is distributed in the hope that it will be useful,
;    but WITHOUT ANY WARRANTY; without even the implied warranty of
;    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;    GNU General Public License for more details.
;
;    You should have received a copy of the GNU General Public License
;    along with this program.  If not, see <http://www.gnu.org/licenses/>.
;

%macro irq 1
[GLOBAL irq%1]
[EXTERN cIRQ%1]
irq%1:
	cli	; no interrupts while we handle this one
	push cIRQ%1
	jmp irqStub
%endmacro

[GLOBAL irq15]
[EXTERN proberam]
irq15:
	cli
	push proberam
	jmp irqStub

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
; irq 15

irqStub:
	pushad
; 	push ebp
	mov ebp, esp
	mov eax, [ebp+32]

	mov dx, ds
	push edx
	mov dx, 0x10	; kernel ring

	mov ds, dx
	mov es, dx
	mov fs, dx
	mov gs, dx

	call eax

	pop edx
	mov ds, dx
	mov es, dx
	mov gs, dx
	mov es, dx

	popad
; 	pop ebp
	add esp, 4	; pop func pointer
	iret	; interrupt return