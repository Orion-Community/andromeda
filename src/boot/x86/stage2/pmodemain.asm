;
;    Main line of the protected mode section.
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

[BITS 32]
%ifndef __STAGE2
[EXTERN kmain]
[GLOBAL pmodemain]
[GLOBAL endptr]
[SECTION .text]

jmp short pmodemain
	dd endptr

pmodemain:
	mov eax, kmain

	jmp eax

.end:
	jmp $

[SECTION .end]
endptr:
	dd 0xdeadbeef
%else

pmodemain:
	
	jmp 0x100000

%endif