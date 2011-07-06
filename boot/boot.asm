CORE_MAGIC	equ	0xC0DEBABE	; Lolz

[SECTION .boot]
[EXTERN  code]                   ; Start of the '.text' section.
[EXTERN  bss]                    ; Start of the .bss section.
[EXTERN  end]
  dd CORE_MAGIC
  dd end

[SECTION .text]
[GLOBAL start]
[EXTERN core]
[EXTERN stack]
start:
  mov ecx, stack		; Set the new stack frame
  add ecx, 0x10000		; Add the size of the stack to the pointer
  mov ebp, ecx
  mov esp, ecx			; Stack grows down in memory and we're at the
  push esp			; top of the minimum required memory
  cli
  call core
  jmp $