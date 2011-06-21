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

[GLOBAL divByZero]
[EXTERN cDivByZero]
divByZero:
	isrNoErr cDivByZero
	jmp isrStub

[GLOBAL nmi]
[EXTERN cNmi]
nmi:
	isrNoErr cNmi
	jmp isrStub

[GLOBAL breakp]
[EXTERN cbp]
breakp:
	isrNoErr cbp
	jmp isrStub

[GLOBAL overflow]
[EXTERN coverflow]
overflow:
	isrNoErr coverflow
	jmp isrStub

[GLOBAL bound]
[EXTERN cBound]
bound:
	isrNoErr cBound
	jmp isrStub

[GLOBAL invalOp]
[extern cInvalOp]
invalOp:
	isrNoErr cInvalOp
	jmp isrStub

[GLOBAL noMath]
[EXTERN cNoMath]
noMath:
	isrNoErr cNoMath
	jmp isrStub

[GLOBAL doubleFault]
[EXTERN cDoubleFault]
doubleFault:
	isrErr cDoubleFault
	jmp isrStub

[GLOBAL depricated]
[EXTERN ignore]
depricated:
	isrNoErr ignore
	jmp isrStub

[GLOBAL invalidTSS]
[EXTERN cInvalidTSS]
invalidTSS:
	isrErr cInvalidTSS
	jmp isrStub

[GLOBAL snp]
[EXTERN cSnp]
snp:
	isrErr cSnp
	jmp isrStub

[GLOBAL stackFault]
[EXTERN cStackFault]
	stackFault:
	isrErr cStackFault
	jmp isrStub

[GLOBAL genProt]
[EXTERN cGenProt]
	genProt:
	isrErr cGenProt
	jmp isrStub

[GLOBAL pageFault]
[EXTERN cPageFault]
pageFault:
	isrErr cPageFault
	jmp isrStub

[GLOBAL fpu]
[EXTERN cFpu]
fpu:
	isrNoErr cFpu
	jmp isrStub

[GLOBAL alligned]
[EXTERN cAlligned]
alligned:
	isrNoErr cAlligned
	jmp isrStub

[GLOBAL machine]
[EXTERN cDoubleFault]
machine:
	isrNoErr cDoubleFault
	jmp isrStub

[GLOBAL simd]
[EXTERN cSimd]
	simd:
	isrNoErr cSimd

isrStub:
	pushad
	mov ebp, esp
	mov eax, [ebp+32]

	mov dx, ds
	push dx
	mov dx, 0x10	; kernel ring

	mov ds, dx
	mov es, dx
	mov fs, dx
	mov gs, dx

	call eax

	pop dx
	mov ds, dx
	mov es, dx
	mov gs, dx
	mov es, dx

	popad
	add esp, 8	; pop error num + routine
	sti
	iret	; interrupt return

%macro isrNoError 1
	cli
	push 0
	push %1
%endmacro

%macro isrError 1
	cli	; the error code is pushed by the cpu
	push %1
%endmacro