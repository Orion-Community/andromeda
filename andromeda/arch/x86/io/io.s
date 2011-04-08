%include "asm/call.mac"

[GLOBAL outb]
[GLOBAL inb]

outb: ; outb (char data, short port)
  enter
  
  xor eax, eax
  xor edx, edx
  
  mov eax, [ebp+12]
  mov edx, [ebp+8]
  
  out dx, al
  
  return

inb: ; inb (short port)
  enter
  
  xor eax, eax
  xor edx, edx
  
  mov edx, [ebp+8]
  
  in al, dx
  
  return