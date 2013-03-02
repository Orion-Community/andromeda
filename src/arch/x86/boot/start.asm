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
[SECTION .boot]
MBOOT_PAGE_ALIGN    equ 1<<0    ; Load kernel and modules on a page boundary
MBOOT_MEM_INFO      equ 1<<1    ; Provide your kernel with memory info
MBOOT_HEADER_MAGIC  equ 0x1BADB002 ; Multiboot Magic value
; NOTE: We do not use MBOOT_AOUT_KLUDGE. It means that GRUB does not
; pass us a symbol table.
MBOOT_HEADER_FLAGS  equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM      equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

GIB_PAGE_TABLES         equ 0x40000 ; Number of page table entries
GIB_PAGE_DIRS           equ 0x100  ; Number of page dir entries
LOW_PAGE_DIRS           equ 0x10
PAGE_TABLE_SIZE         equ 0x1000 ; Size of page table in bytes

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

gdt:
    times 8 db 0
    UNREAL_SEG equ $ - gdt      ; Data segment, read/write, expand down
        dw 0FFFFh
        dw 0
        db 0
        db 0x92
        db 0xCF
        db 0
gdt_end equ $ - gdt

gdtr:
        dw gdt_end - 1; gdt limit = size
        dd gdt ; gdt base address

[GLOBAL start]
start:
  cli                   ; Interrupts not allowed
  mov esp, boot_stack   ; Set up a simple temporary stack
  add esp, 0x400        ; Move it to the right location

  push eax              ; Push grub data
  push ebx
  push ecx
  push edx

  call boot_setup_paging ; Set up basic paging

  pop edx               ; Pop grub data
  pop ecx
  pop ebx
  pop eax

;   lgdt[gdtr]

  ; jump to the higher half kernel
  jmp high_start

boot_setup_paging:
  push ebp
  mov ebp, esp

  xor eax, eax
  xor ebx, ebx

; Configure all the page tables in one single go (GIB_PAGE_TABLES)
.1:
  mov ecx, ebx
  or ecx, 3 ; Set some bits!!!
  mov [page_table_boot+eax*4], ecx
  add ebx, PAGE_TABLE_SIZE
  inc eax
  cmp eax, GIB_PAGE_TABLES
  jne .1

; Set up page directory pointer
  mov ebx, page_dir_boot
  mov cr3, ebx

; Build the page directory
  xor ecx, ecx
  mov eax, page_table_boot
  or eax, 3

; The first set of page directory entries (Maps the 1GiB range linearly)
; Registers should be correct already, so lets loop
.2:
  mov [ebx], eax
  add ebx, 4
  add eax, PAGE_TABLE_SIZE
  inc ecx
  cmp ecx, LOW_PAGE_DIRS
  jne .2

; The 3 GiB part, map the entire first GIB to 3 GIB
; The C code that is to execute next, will nuance this, but for now it is safest
; to just map the entire potential space.
; Set up the registers
  mov ebx, page_dir_boot
  add ebx, 0xC00
  xor ecx, ecx
  mov eax, page_table_boot
  or eax, 3

; The actual loop
.3:
  mov [ebx], eax
  add ebx, 4
  add eax, PAGE_TABLE_SIZE
  inc ecx
  cmp ecx, GIB_PAGE_DIRS
  jne .3

; Set the PG bit
  mov eax, cr0
  or eax, 0x80000000
  mov cr0, eax ; We're in virtual memory now!

  mov esp, ebp
  pop ebp
  ret

boot_stack:
  times 0x400 db 0

[SECTION .PD]
; page_dir_boot: ; This is basically the page table
; times 0x400 dd 0x0
;
; page_table_boot: ; And this will be mapped to for both 0 - 4MiB and 3 - 3.004GiB
; times 0x40000 dd 0x0

[EXTERN page_dir_boot]
[EXTERN page_table_boot]

[SECTION .higherhalf]           ; Defined as start of image for the C kernel
[GLOBAL higherhalf]
higherhalf:
  dd 0

[SECTION .text]
[GLOBAL  high_start]                  ; Kernel entry point.
[EXTERN  init]                  ; This is the entry point of our C code
[EXTERN  stack]

high_start:
    ; Load multiboot information:
    mov ecx, stack		; Set the new stack frame
    add ecx, 0x8000		; Add the size of the stack to the pointer
    mov ebp, ecx
    mov esp, ecx		; Stack grows down in memory and we're at the
    push esp			; top of the minimum required memory
    push ebx
    push eax

    ; Execute the kernel:
    call init                   ; call our init() function.
    jmp $                       ; Enter an infinite loop, to stop the processor
                                ; executing whatever rubbish is in the memory
                                ; after our kernel!
