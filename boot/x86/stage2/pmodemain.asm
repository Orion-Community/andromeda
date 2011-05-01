[BITS 32]
[EXTERN kmain]
[SECTION .text]

pmodemain:
; 	mov byte [ds:0xB8002], 'P'      ; Move the ASCII-code of 'P' into first video memory
;       mov byte [ds:0xB8003], 1Bh      ; Assign a color code
	;int 0x19
	call kmain
	jmp $
