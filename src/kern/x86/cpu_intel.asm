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

[SECTION .data]
[GLOBAL lock]
	lock db 0

[SECTION .text]
[GLOBAL geteflags]
geteflags:
	push ebp
	mov ebp, esp
	
	pushfd
	pop eax

	mov esp, ebp
	pop ebp
	ret

[GLOBAL mutex_lock]
mutex_lock:
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

[GLOBAL mutex_release]
mutex_release:
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
