%include "asm/call.mac"
%include "asm/isr.mac"

[GLOBAL divByZero]
[EXTERN cDivByZero]
divByZero:
  isrNoErr cDivByZero
  jmp commonStub
  
[GLOBAL nmi]
[EXTERN cNmi]
nmi:
  isrNoErr cNmi
  jmp commonStub

[GLOBAL breakp]
[EXTERN cbp]
breakp:
  isrNoErr cbp
  jmp commonStub

[GLOBAL overflow]
[EXTERN coverflow]
overflow:
  isrNoErr coverflow
  jmp commonStub

[GLOBAL bound]
[EXTERN cBound]
bound:
  isrNoErr cBound
  jmp commonStub

[GLOBAL invalOp]
[extern cInvalOp]
invalOp:
  isrNoErr cInvalOp
  jmp commonStub

[GLOBAL noMath]
[EXTERN cNoMath]
noMath:
  isrNoErr cNoMath
  jmp commonStub

[GLOBAL doubleFault]
[EXTERN cDoubleFault]
doubleFault:
  isrErr cDoubleFault
  jmp commonStub

[GLOBAL depricated]
[EXTERN ignore]
depricated:
  isrNoErr ignore
  jmp commonStub

[GLOBAL invalidTSS]
[EXTERN cInvalidTSS]
invalidTSS:
  isrErr cInvalidTSS
  jmp commonStub

[GLOBAL snp]
[EXTERN cSnp]
snp:
  isrErr cSnp
  jmp commonStub

[GLOBAL stackFault]
[EXTERN cStackFault]
stackFault:
  isrErr cStackFault
  jmp commonStub

[GLOBAL genProt]
[EXTERN cGenProt]
genProt:
  isrErr cGenProt
  jmp commonStub

[GLOBAL pageFault]
[EXTERN cPageFault]
pageFault:
  isrErr cPageFault
  jmp commonStub

[GLOBAL fpu]
[EXTERN cFpu]
fpu:
  isrNoErr cFpu
  jmp commonStub

[GLOBAL alligned]
[EXTERN cAlligned]
alligned:
  isrNoErr cAlligned
  jmp commonStub

[GLOBAL machine]
machine:
  isrNoErr cDoubleFault
  jmp commonStub

[GLOBAL simd]
[EXTERN cSimd]
simd:
  isrNoErr cSimd
  ;jmp commonStub

commonStub:
  pushad
  mov eax, [esp+32]
  
  mov dx, ds
  push edx
  mov edx, 0x10
  mov ds, dx
  mov es, dx
  mov fs, dx
  mov gs, dx
  
  call eax
  
  pop edx
  mov ds, dx
  mov es, dx
  mov fs, dx
  mov gs, dx
  
  popad
  
  add esp, 8
  
  iret

[GLOBAL loadIdt]
loadIdt:
  enter
  
  mov eax, [ebp+8]
  lidt [eax]
  
  int3
  
  return