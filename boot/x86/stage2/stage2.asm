[BITS 16]
[SECTION .data]
;
; GLOBAL DESCRIPTOR TABLE
;
gdt:
    times 8 db 0
    CODE_SEG equ $ - gdt	; Code segment, read/execute, nonconforming
        dw 0FFFFh
        dw 0
        db 0
        db 0x9A
        db 0xCF
        db 0
    DATA_SEG equ $ - gdt	; Data segment, read/write, expand down
        dw 0FFFFh
        dw 0
        db 0
        db 0x92
        db 0xCF
        db 0
gdt_end: ; pointer to the end of the gdt

gdtr:
	dw gdt_end - gdt - 1; gdt limit = size
	dd gdt ; gdt base address

	; Status messages
	pmode db 'Implementing a GDT and PMode', 0x0

[SECTION .stage2]
[GLOBAL endptr]

main:
	mov si, pmode
	call println

	cli
	lgdt [gdtr]
	mov eax, cr0
	or eax, 1
	mov cr0, eax

	mov ax, DATA_SEG
	mov ds, ax
	mov ss, ax
	jmp CODE_SEG:pmodemain


;
; Print routines
;

%include 'boot/x86/println.asm'
%include 'boot/x86/stage2/pmodemain.asm'

[SECTION .end]
endptr:
	dw 0xBEEF