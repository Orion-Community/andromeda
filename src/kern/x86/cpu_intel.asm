;
;    The openLoader project - CPU specific routines
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

[SECTION .text]
[GLOBAL ol_get_eflags]
ol_get_eflags:
	push ebp
	mov ebp, esp
	
	pushfd
	pop eax

	mov esp, ebp
	pop ebp
	ret

[GLOBAL ol_set_eflags]
ol_set_eflags:
	push ebp
	mov ebp, esp
	
	mov eax, [ebp+8]
	push eax
	popfd

	mov esp, ebp
	pop ebp
	ret

[GLOBAL ol_mutex_lock]
ol_mutex_lock:
	push ebp
	mov ebp, esp

	mov eax, 1
	
.lock:
	xor eax, [ebp+8]
	xor [ebp+8], eax
	xor eax, [ebp+8]
	test eax, eax
	jnz .lock

	mov esp, ebp
	pop ebp
	ret

[GLOBAL ol_mutex_release]
ol_mutex_release:
	push ebp
	mov ebp, esp

	mov eax, 0
	
.lock:
	xor eax, [ebp+8]
	xor [ebp+8], eax
	xor eax, [ebp+8]
	test eax, eax
	jnz .lock

	mov esp, ebp
	pop ebp
	ret
