;
;    Low level functions of the GoldenEagle Bootloader standard library.
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

[SECTION .data]
fakeidt:	dw 0x0
		dd 0x0
[SECTION .text]

%include "sys/include/stdlib.asmh"

[GLOBAL endprogram]
endprogram:
	cli
	hlt
	jmp halt
	ret

[GLOBAL halt]
halt:
	hlt
	ret

[GLOBAL getregs]
getregs:
	mov [regs], eax
	mov [regs+4], ebx
	mov [regs+8], ecx
	mov [regs+12], edx
	mov [regs+20], esi
	mov [regs+24], edi
	mov [regs+28], ebp
	mov [regs+32], esp

	mov eax, regs
	ret

[GLOBAL getsegs]
getsegs:
	mov [segs], ds
	mov [segs+2], cs
	mov [segs+4], es
	mov [segs+6], fs
	mov [segs+8], gs
	mov [segs+10], ss

	mov eax, segs
	ret

[GLOBAL setInterrupts]
setInterrupts:
	sti
	ret

[GLOBAL clearInterrupts]
clearInterrupts:
	cli
	ret

[GLOBAL reboot]
reboot:
	sti
	lidt [fakeidt]
	ret

[EXTERN isSleeping]
[EXTERN sleepTime]
[GLOBAL sleep]
sleep:
	push ebp
	mov ebp, esp
	push eax
	mov eax, [ebp+8] ; first argument -> mili's to sleep
	mov [sleepTime], eax
	mov [isSleeping], byte 1

.sleepLoop:
	cli
	mov eax, [sleepTime]
	or eax, eax
	jz .done

	sti
	times 8 nop
	jmp .sleepLoop

.done:
	mov [isSleeping], byte 0
	pop eax
	pop ebp
	ret
