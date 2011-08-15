;
;    This is the PS/2 controller, which cares about the low level hardware IO.
;    High level drivers relay on these functions.
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

%include "sys/ps2.asmh"
%include "stdlib.asmh"

[SECTION .text]

; write a byte to the kbc. 
[GLOBAL ps2write]
ps2write: ; bool ps2write(uint8_t val)
	pushad
	mov ebp, esp
	mov ecx, 6	; give it 5 tries
	
.looptop:
	dec cx
	jz .fail

	mov dl, byte [ebp+36]
	call ps2await_ack
	xor al, OL_PS2_RESEND
	jz .looptop
	mov eax, TRUE
	jmp .end
	

.fail:
	xor eax, eax

.end:
	add esp, 8
	pop ebp
	add esp, 4
	pop ebx
	pop edx
	pop ecx
	add esp, 4
	ret

; ------------------------------------------------------------------------------

[GLOBAL ps2read]
ps2read:
	pushad

	mov ecx, 6	; 5 tries

.looptop:
	xor eax, eax
	dec cx
	jz .end
	call ps2readwait

	jz .looptop

	in al, OL_PS2_DATA_PORT
	and eax, 0xff
	
.end:
	pop edi
	pop esi
	pop ebp
	add esp, 4 ; 'pop' esp
	pop ebx
	pop edx
	pop ecx
	add esp, 4 ; 'pop' eax

	ret

; ------------------------------------------------------------------------------

; static routine which waits for the acknowledge byte to come in. If the read the
; ack byte, it will return it in eax. On failure, it will return zero
; It expects the value to write in dl.
ps2await_ack:
	pushad
	mov ecx, 6	; give it up to 4 tries

.looptop:
	xor eax, eax
	dec ecx
	jz .end

	call ps2writewait	; wait for write
	test al, al
	jz .looptop	; returns non-zero in al if you can write

	mov al, dl
	out OL_PS2_DATA_PORT, al	; write requested data
	mov ecx, 6	; try 5 times

.looptop2:
	xor eax, eax
	dec cx
	jz .end

	call ps2readwait
	test al, al
	jz .looptop2
	
	in al, OL_PS2_DATA_PORT
	and eax, 0xff

.end:
	add esp, 24 ; pop edi - edx
	pop ecx
	add esp, 4 ; 'pop' eax
	ret

; ------------------------------------------------------------------------------

; poll the status bit - if it doesn't clear it returns non-zero in eax
ps2writewait:
	pushad
	mov ecx, 0x80001

.looptop:
	xor eax, eax
	dec ecx
	jz .end
	in al, OL_PS2_STATUS_REGISTER
	test al, 2
	jnz .looptop
	mov al, 1
.end:
	add esp, 24 ; pop edi - edx
	pop ecx
	add esp, 4 ; 'pop' eax
	ret

; ------------------------------------------------------------------------------

; poll the status bit - when OK, it returns non-zero
ps2readwait:
	pushad
	xor al, al
	xor ecx, ecx
	mov ecx, 0x80001


.looptop:
	xor eax, eax
	dec ecx
	jz .end
	in al, OL_PS2_STATUS_REGISTER
	test al, 1
	jz .looptop
	mov al, 1
.end:
	add esp, 24 ; pop edi - edx
	pop ecx
	add esp, 4 ; 'pop' eax
	ret
