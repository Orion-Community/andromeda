; The assembly headers for the interrupts.
[GLOBAL sti]
[GLOBAL cli]
[GLOBAL DetectAPIC]
[GLOBAL getVendor]
[GLOBAL getCS]

%include "asm/call.mac"

sti: ; Start interrupts from C level code
  enter
  
  sti ; Start interrupts
  
  return
  
cli: ; Shut down interrupts from C level code
  enter 
  
  cli ; Stop interrupts
  
  return
  
DetectAPIC:
  enter
  
  call getVendor
  cmp eax, 1
  jnz test
  cmp eax, 2
  jnz test
  jmp err
  
test:
  mov eax, 1 ; prepare CPUID
  cpuid ; Issue CPUID
  
  xor edx, 0x100 ; mask the flag out
  mov eax, edx ; return value to eax
  
  return
    
err: ; invalid CPUID
  xor eax, eax ; return 0
  return
  
getVendor:
  enter
  xor eax, eax
  cpuid
  
  cmp ebx, "Genu"
  jz intel
  cmp ebx, 0x68747541
  jz amdTest
  xor eax, eax
  return
  
intel:
  mov eax, 1
  return
  
amdTest:
  mov eax, 2
  return

getCS:
  enter
  xor eax, eax
  mov ax, cs
  return