;   Orion OS, The educational operatingsystem
;   Copyright (C) 2011  Bart Kuivenhoven

;   This program is free software: you can redistribute it and/or modify
;   it under the terms of the GNU General Public License as published by
;   the Free Software Foundation, either version 3 of the License, or
;   (at your option) any later version.

;   This program is distributed in the hope that it will be useful,
;   but WITHOUT ANY WARRANTY; without even the implied warranty of
;   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;   GNU General Public License for more details.

;   You should have received a copy of the GNU General Public License
;   along with this program.  If not, see <http://www.gnu.org/licenses/>.

;
;	The multiboot header file, calls the C-level entry point
;
;%ifdef __COMPRESSED
[SECTION .boot]
MBOOT_PAGE_ALIGN    equ 1<<0    ; Load kernel and modules on a page boundary
MBOOT_MEM_INFO      equ 1<<1    ; Provide your kernel with memory info
MBOOT_HEADER_MAGIC  equ 0x1BADB002 ; Multiboot Magic value
; NOTE: We do not use MBOOT_AOUT_KLUDGE. It means that GRUB does not
; pass us a symbol table.
MBOOT_HEADER_FLAGS  equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM      equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)


[BITS 32]                       ; All instructions should be 32-bit.

[GLOBAL mboot]                  ; Make 'mboot' accessible from C.
[EXTERN code]                   ; Start of the '.text' section.
[EXTERN bss]                    ; Start of the .bss section.
[EXTERN end]                    ; End of the last loadable section.

mboot:
    dd  MBOOT_HEADER_MAGIC      ; GRUB will search for this value on each
                                ; 4-byte boundary in your kernel file
    dd  MBOOT_HEADER_FLAGS      ; How GRUB should load your file / settings
    dd  MBOOT_CHECKSUM          ; To ensure that the above values are correct

    dd  mboot                   ; Location of this descriptor
    dd  code                    ; Start of kernel '.text' (code) section.
    dd  bss                     ; End of kernel '.data' section.
    dd  end                     ; End of kernel.
    dd  start                   ; Kernel entry point (initial EIP).

[GLOBAL start]
start:
  lgdt [trickgdt]
  mov dx, 0x10
  mov ds, dx
  mov es, dx
  mov fs, dx
  mov gs, dx
  mov ss, dx

  ; jump to the higher half kernel
  jmp 0x08:high_start

trickgdt:
        dw gdt_end - gdt - 1 ; size of the GDT
        dd gdt ; linear address of GDT

gdt:
        dd 0, 0                                                 ; null gate
        db 0xFF, 0xFF, 0, 0, 0, 10011010b, 11001111b, 0x40
; code selector 0x08: base 0x40000000, limit 0xFFFFFFFF, type 0x9A,
                                                               ;granularity 0xCF
        db 0xFF, 0xFF, 0, 0, 0, 10010010b, 11001111b, 0x40
; data selector 0x10: base 0x40000000, limit 0xFFFFFFFF, type 0x92,
                                                               ;granularity 0xCF

gdt_end:

[SECTION .higherhalf]           ; Defined as start of image for the C kernel
[GLOBAL begin]
begin:
  dd 0

[SECTION .text]
[GLOBAL  high_start]                  ; Kernel entry point.
[EXTERN  init]                  ; This is the entry point of our C code
[EXTERN  stack]

high_start:
    ; Load multiboot information:
    mov ecx, 0x10
    mov ss, ecx
    mov ecx, stack		; Set the new stack frame
    add ecx, 0x8000		; Add the size of the stack to the pointer
    mov ebp, ecx
    mov esp, ecx		; Stack grows down in memory and we're at the
    push esp			; top of the minimum required memory
    push ebx
    push eax

    ; Execute the kernel:
    cli                         ; Forbidden for interrupts.
    call init                   ; call our init() function.
    jmp $                       ; Enter an infinite loop, to stop the processor
                                ; executing whatever rubbish is in the memory
                                ; after our kernel!
; %else
; CORE_MAGIC	equ	0xC0DEBABE	; Lolz
; 
; [SECTION .boot]
; [EXTERN  code]                   ; Start of the '.text' section.
; [EXTERN  bss]                    ; Start of the .bss section.
; [EXTERN  end]
;   dd CORE_MAGIC
;   dd end
; 
; [SECTION .text]
; [GLOBAL  start]
; [EXTERN  kmain]
; start:
;   cli
;   call kmain
;   jmp $
; %endif
