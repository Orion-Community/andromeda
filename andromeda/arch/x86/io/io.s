[GLOBAL outb]
[GLOBAL inb]

outb: ; outb (char data, short port)
  push ebp
  mov ebp, esp
  
  xor eax, eax
  xor edx, edx
  
  mov eax, [ebp+8]
  mov edx, [ebp+12]
  
  out dx, al
  
  mov esp, ebp
  pop ebp
  ret

inb: ; inb (short port)
  push ebp
  mov ebp, esp
  
  xor eax, eax
  xor edx, edx
  
  mov edx, [ebp+8]
  
  in al, dx
  
  mov esp, ebp
  pop ebp
  ret