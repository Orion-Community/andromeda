/*
    Orion OS, The educational operatingsystem
    Copyright (C) 2011  Bart Kuivenhoven

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <interrupts.h>
#include <stdlib.h>

void checkFrame(isrVal_t* regs)
{
  if (regs->cs != 0x8 && regs->cs != 0x18)
  {
    printhex(regs->cs); putc('\n');
    panic("Incorrect CS!");
  }
  else if (regs->ds != 0x10 && regs->ds != 0x20)
  {
    printhex(regs->ds); putc('\n');
    panic("Incorrect DS!");
  }
}
void cDivByZero(isrVal_t regs)
{
  checkFrame(&regs);
  if (regs.cs != 0x8)
  {
    panic("No process killing code yet");
  }
  printf("\nDiv by 0\neip\tcs\teflags\tprocesp\tss\n");
  printhex(regs.eip); putc('\t');
  printhex(regs.cs); putc('\t');
  printhex(regs.eflags); putc('\t');
  printhex(regs.procesp); putc('\t');
  printhex(regs.ss); putc('\n');
  printf("\nCurrent:\n");
  printf("CS\tDS\tSS\tESP\n");
  printhex(getCS());		putc('\t');
  printhex(getDS());		putc('\t');
  printhex(getSS());		putc('\t');
  printhex(getESP());		putc('\n');
//   panic ("Devide by zero");
}

void cNmi(isrVal_t regs)
{
  checkFrame(&regs);
  panic("Don't know what a non-maskable interrupt does!!!");
}

void cbp(isrVal_t regs)
{
  checkFrame(&regs);
  printf("Debug:\n");

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

  printf("\nerr_code\tfunc_ptr\n");
  printhex(regs.errCode);	putc('\t');
  printhex(regs.funcPtr);	putc('\n');
  
  printf("\n\nCurrent:\n");
  printf("CS\tDS\tSS\tESP\n");
  printhex(getCS());		putc('\t');
  printhex(getDS());		putc('\t');
  printhex(getSS());		putc('\t');
  printhex(getESP());		putc('\n');
}

void coverflow(isrVal_t regs)
{
  checkFrame(&regs);
  panic("Overflow");
}

void cBound(isrVal_t regs)
{
  checkFrame(&regs);
  panic("Bounds");
}

void cInvalOp(isrVal_t regs)
{
  checkFrame(&regs);
  panic("Invalid Opcode!");
}

void cNoMath(isrVal_t regs)
{
  checkFrame(&regs);
  panic("No math coprocessor");
}

void cDoubleFault(isrVal_t regs)
{
  checkFrame(&regs);
  panic("Double fault");
}

void ignore(isrVal_t regs)
{
  checkFrame(&regs);
}

void cInvalidTSS(isrVal_t regs)
{
  checkFrame(&regs);
  panic("Invalid TSS");
}

void cSnp(isrVal_t regs)
{
  checkFrame(&regs);
  panic("Stack not present!");
}
void cStackFault(isrVal_t regs)
{
  checkFrame(&regs);
  panic("Stack fault!");
}

void cGenProt(isrVal_t regs)
{
  checkFrame(&regs);
  printf("\nGeneral Protection Fault\neip\tcs\teflags\tprocesp\tss\n");
  printhex(regs.eip); putc('\t');
  printhex(regs.cs); putc('\t');
  printhex(regs.eflags); putc('\t');
  printhex(regs.procesp); putc('\t');
  printhex(regs.ss); putc('\n');
  printf("\nCurrent:\n");
  printf("CS\tDS\tSS\tESP\n");
  printhex(getCS());		putc('\t');
  printhex(getDS());		putc('\t');
  printhex(getSS());		putc('\t');
  printhex(getESP());		putc('\n');
//   panic("General Protection fault");
}

void cPageFault(isrVal_t regs)
{
  checkFrame(&regs);
  panic("Paging isn't finished yet");
}

void cFpu(isrVal_t regs)
{
  checkFrame(&regs);
  panic("Floating point exception");
}

void cAlligned(isrVal_t regs)
{
  checkFrame(&regs);
  panic("Alligned exception");
}

void cSimd(isrVal_t regs)
{
  checkFrame(&regs);
  panic("SSE exception");
}