;
;    The interrupt service routine headers.
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

%macro isrNoError 1
	cli
	push 0
	push %1
%endmacro

%macro isrError 1
	cli	; the error code is pushed by the cpu
	push %1
%endmacro

[GLOBAL divByZero]
[EXTERN cDivByZero]
divByZero:
	isrNoError cDivByZero
	jmp isrStub

[GLOBAL nmi]
[EXTERN cNmi]
nmi:
	isrNoError cNmi
	jmp isrStub

[GLOBAL breakp]
[EXTERN cBreakp]
breakp:
	isrNoError cBreakp
	jmp isrStub

[GLOBAL overflow]
[EXTERN cOverflow]
overflow:
	isrNoError cOverflow
	jmp isrStub

[GLOBAL bound]
[EXTERN cBound]
bound:
	isrNoError cBound
	jmp isrStub

[GLOBAL invalOp]
[extern cInvalOp]
invalOp:
	isrNoError cInvalOp
	jmp isrStub

[GLOBAL noMath]
[EXTERN cNoMath]
noMath:
	isrNoError cNoMath
	jmp isrStub

[GLOBAL doubleFault]
[EXTERN cDoubleFault]
doubleFault:
	isrError cDoubleFault
	jmp isrStub

[GLOBAL depricated]
[EXTERN cDepricated]
depricated:
	isrNoError cDepricated
	jmp isrStub

[GLOBAL invalidTSS]
[EXTERN cInvalidTSS]
invalidTSS:
	isrError cInvalidTSS
	jmp isrStub

[GLOBAL snp]
[EXTERN cSnp]
snp:
	isrError cSnp
	jmp isrStub

[GLOBAL stackFault]
[EXTERN cStackFault]
	stackFault:
	isrError cStackFault
	jmp isrStub

[GLOBAL genProt]
[EXTERN cGenProt]
	genProt:
	isrError cGenProt
	jmp isrStub

[GLOBAL pageFault]
[EXTERN x86_pagefault]
pageFault:
	isrError x86_pagefault
	jmp isrStub

[GLOBAL fpu]
[EXTERN cFpu]
fpu:
	isrNoError cFpu
	jmp isrStub

[GLOBAL alligned]
[EXTERN cAlligned]
alligned:
	isrNoError cAlligned
	jmp isrStub

[GLOBAL machine]
[EXTERN cDoubleFault]
machine:
	isrNoError cDoubleFault
	jmp isrStub

[GLOBAL simd]
[EXTERN cSimd]
	simd:
	isrNoError cSimd

isrStub:
	pushad
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
	add esp, 8	; pop error num + routine
	sti
	iret	; interrupt return
