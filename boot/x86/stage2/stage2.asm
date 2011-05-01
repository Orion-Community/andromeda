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
gdt_end equ $ - gdt ; pointer to the end of the gdt

gdtr:
	dw gdt_end - 1; gdt limit = size
	dd gdt ; gdt base address

	; Status messages
	pmode db 'Implementing a GDT and PMode', 0x0

[SECTION .stage2]
[GLOBAL endptr]

main:
	mov si, pmode
	call println

	cli
	xor ax, ax
	mov ds, ax

	lgdt [gdtr]

.flushdataseg:
	mov ax, 0x10
	mov ds, ax
	mov ss, ax
	mov es, ax

.enterpmode:
	mov eax, cr0
	or eax, 00000001b
	mov cr0, eax

.flushcodeseg:
	jmp 0x8:pmodemain	

.hang:
	jmp $
;
; Print routines
;

times 1333 db 0

%include 'boot/x86/println.asm'
%include 'boot/x86/stage2/pmodemain.asm'

[SECTION .end]
endptr:
	dw 0xBEEF