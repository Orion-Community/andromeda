[GLOBAL inb]
[GLOBAL outb]
[SECTION .text]
inb:
	push ebp
	mov ebp, esp
	
	xor eax, eax
	mov dx, word [ebp+8] ; port
	in al, dx

	pop ebp
	ret

outb:
	push ebp
	mov ebp, esp
	
	mov al, byte [ebp+8] ; data
	mov dx, word [ebp+12] ; port
	out dx, al

	pop ebp
	ret