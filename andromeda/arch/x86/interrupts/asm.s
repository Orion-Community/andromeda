; The assembly headers for the interrupts.

[GLOBAL sti]
[GLOBAL cli]
[GLOBAL DetectAPIC]

sti: ; Start interrupts from C level code
  push ebp ; Push the stack frame, is faster than enter 0,0
  mov ebp, esp
  
  sti ; Start interrupts
  
  mov esp, ebp ; reset stack frame, is faster than leave
  pop ebp
  ret
  
cli: ; Shut down interrupts from C level code
  push ebp ; Save the stack frame
  mov ebp, esp
  
  cli ; Stop interrupts
  
  mov esp, ebp ; reset stack frame
  pop ebp
  ret
  
DetectAPIC:
  push ebp ;push stack frame
  mov ebp, esp
  
  mov eax, 1 ; prepare CPUID
  CPUID ; Issue CPUID
  
  xor edx, 0x100 ; mask the flag out
  mov eax, edx ; return value to eax
  
  mov esp, ebp ; reset stack frame
  pop ebp
  ret