;
;    The PIT will be reprogrammed to 100 hertz.
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

[SECTION .bss]

[GLOBAL IRQ0_miliS]
	IRQ0_miliS resd 1

[GLOBAL IRQ0_fractions]
	IRQ0_fractions resd 1

[SECTION .text]

[EXTERN iowait]

%include "interrupts/pit.asmh"

[GLOBAL initPIT]
initPIT:	; void initPIT(uint16 hz);
	push ebp
	mov ebp, esp
	pushad
	mov ebx, [ebp+8]	; ebx contains amount of hertz

	cmp ebx, OL_MIN_FREQ
	mov eax, 0xffff
	jbe .calcFreq

	cmp ebx, OL_MAX_FREQ
	mov eax, 1
	jae .calcFreq

; calculate the reload value
; the max frequenty is not accurate. using OL_RELOAD_DIVISOR / 3 is more accurate
	mov eax, OL_RELOAD_DIVISOR
	xor edx ,edx
	div ebx
	cmp edx, OL_RELOAD_DIVISOR / 2	; more then half? yes >> increase with one
	jb .l1
	inc eax

.l1:
	mov ebx, 3
	xor edx, edx
	div ebx		; eax = OL_RELOAD_DIVISOR / hertz / 3
	cmp edx, 3/2	; more then half?
	jb .calcFreq
	inc eax

 ; Calculate the amount of time between IRQs in 32.32 fixed point
 ;
 ; Note: The basic formula is:
 ;           time in ms = reload_value / (3579545 / 3) * 1000
 ;       This can be rearranged in the follow way:
 ;           time in ms = reload_value * 3000 / 3579545
 ;           time in ms = reload_value * 3000 / 3579545 * (2^42)/(2^42)
 ;           time in ms = reload_value * 3000 * (2^42) / 3579545 / (2^42)
 ;           time in ms * 2^32 = reload_value * 3000 * (2^42) / 3579545 / (2^42) * (2^32)
 ;           time in ms * 2^32 = reload_value * 3000 * (2^42) / 3579545 / (2^10)

.calcFreq:
	push eax
	mov ebx, eax
	mov eax, 0xDBB3A062		;eax = 3000 * (2^42) / 3579545
	mul ebx				;edx:eax = reload_value * 3000 * (2^42) / 3579545
	shrd eax,edx, 10
	shr edx, 10			;edx:eax = reload_value * 3000 * (2^42) / 3579545 / (2^10)

	mov [IRQ0_miliS], edx		;Set whole mS between IRQs
	mov [IRQ0_fractions], eax	;Set fractions of 1 mS between IRQs

.programPIT:
	pop edx		; edx = reload value
	pushfd
	cli

	mov al,00110100b	;channel 0, lobyte/hibyte, rate generator
	out OL_PIT_COMMAND, al
	call iowait

	mov eax, edx
	out OL_PIT_CHAN0_DATA, al
	shr ax, 8
	out OL_PIT_CHAN0_DATA, al

	popfd

.done:
	popad
	pop ebp
	ret
