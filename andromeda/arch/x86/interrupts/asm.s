; The assembly headers for the interrupts.

[GLOBAL sti]
[GLOBAL cli]
[GLOBAL DetectAPIC]
[GLOBAL getVendor]

%macro enter 0
  push ebp
  mov ebp, esp
%endmacro

%macro return 0
  mov esp, ebp
  pop ebp
  ret
%endmacro

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
  
  mov eax, 0
  cpuid ; Test for genuine intel

  mov eax, "Genu"
  cmp ebx, eax
  jnz amd
  
test:
  mov eax, 1 ; prepare CPUID
  cpuid ; Issue CPUID
  
  xor edx, 0x100 ; mask the flag out
  mov eax, edx ; return value to eax
  
  return
  
amd: ; Not genuine intel
  xor eax, eax
  cpuid
  
  mov eax, 0x68747541
  cmp ebx, eax
  jnz err
  
  mov eax, 1
  
  jmp test
  
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