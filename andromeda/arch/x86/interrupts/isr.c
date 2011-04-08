#include <interrupts.h>
#include <error/panic.h>
#include <text.h>
void cDivByZero(isrVal_t regs)
{
  if (regs.ds > 0x10)
  {
    panic("No process killing code yet");
  }
  panic ("Devide by zero");
}

void cNmi(isrVal_t regs)
{
  panic("Don't know what a non-maskable interrupt does!!!");
}

void cbp(isrVal_t regs)
{
  printf("Debug:\n");

  if (regs.procesp == 0x55aa55aa)
  {
    printf((char*)regs.eax); putc('\n');
  }

  printf("eax\tebx\tecx\tedx\n");
  printhex(regs.eax);	putc('\t');
  printhex(regs.ebx);	putc('\t');
  printhex(regs.ecx);	putc('\t');
  printhex(regs.edx);	putc('\n');

  printf("\nds\n");
  printhex(regs.ds);	putc('\t');

  printf("\nedi\tesi\tebp\tesp\n");
  printhex(regs.edi);	putc('\t');
  printhex(regs.esi);	putc('\t');
  printhex(regs.ebp);	putc('\t');
  printhex(regs.esp);	putc('\n');

  printf("\neip\tcs\teflags\tuseresp\tss\n");
  printhex(regs.eip);		putc('\t');
  printhex(regs.cs);		putc('\t');
  printhex(regs.eflags);	putc('\t');
  printhex(regs.procesp);	putc('\t');
  printhex(regs.ss);		putc('\n');

  printf("\nerr_code\n");
  printhex(regs.errCode);	putc('\n');
}

void coverflow(isrVal_t regs)
{
  panic("Overflow");
}

void cBound(isrVal_t regs)
{
  panic("Bounds");
}

void cInvalOp(isrVal_t regs)
{
  panic("Invalid Opcode!");
}

void cNoMath(isrVal_t regs)
{
  panic("No math coprocessor");
}

void cDoubleFault(isrVal_t regs)
{
  panic("Double fault");
}

void ignore(isrVal_t regs)
{
}

void cInvalidTSS(isrVal_t regs)
{
  panic("Invalid TSS");
}

void cGenProt(isrVal_t regs)
{
  panic("General Protection fault");
}

void cPageFault(isrVal_t regs)
{
  panic("Paging isn't finished yet");
}

void cFpu(isrVal_t regs)
{
  panic("Floating point exception");
}

void cAlligned(isrVal_t regs)
{
  panic("Alligned exception");
}

void cSimd(isrVal_t regs)
{
  panic("SSE exception");
}