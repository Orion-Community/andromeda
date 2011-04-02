; The assembly headers for the interrupts.

[GLOBAL sti]
[GLOBAL cli]

sti: ; Start interrupts from C level code
  push ebp
  mov ebp, esp
  
  sti
  
  mov esp, ebp
  pop ebp
  ret
  
cli: ; Shut down interrupts from C level code
  push ebp
  mov ebp, esp
  
  cli
  
  mov esp, ebp
  pop ebp
  ret