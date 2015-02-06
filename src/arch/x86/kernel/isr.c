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

#include <arch/x86/interrupts.h>
#include <stdlib.h>
#include <mm/paging.h>
#include <andromeda/cpu.h>
#include <arch/x86/task.h>
#include <andromeda/syscall.h>
#include <andromeda/system.h>

/*
extern uint32_t pgbit;
*/
void checkFrame(isrVal_t* regs)
{
        if ((regs->cs&0xFFFF) != 0x8 && (regs->cs&0xFFFF) != 0x18) {
                printf("%X\n", regs->cs);
                panic("Incorrect CS!");
        } else if ((regs->ds&0xFFFF) != 0x10 && (regs->ds&0xFFFF) != 0x20) {
                printf("%X\n", regs->ds);
                panic("Incorrect DS!");
        }
}

void cDivByZero(isrVal_t* regs)
{
        do_interrupt(0);
        printf("D0\n");
        checkFrame(regs);
        if (regs->cs != 0x8)
        {
                panic("No process killing code yet");
        }
        printf("\nDiv by 0\neip\tcs\teflags\tprocesp\tss\n");
        printf("%X\t%X\t%X\t%X\t%X\n", regs->eip, regs->cs, regs->eflags, regs->procesp, regs->ss);
        printf("\nCurrent:\n");
        printf("CS\tDS\tSS\tESP\n");
        printf("%X\t%X\t%X\t%X\n", getCS(), getDS(), getSS(), getESP());
        panic ("Devide by zero");
}

void cNmi(isrVal_t* regs)
{
        do_interrupt(2);
        printf("NMI\n");
        checkFrame(regs);
        panic("Don't know what a non-maskable interrupt does!!!");
}

void cBreakp(isrVal_t* regs)
{
        do_interrupt(3);
        printf("BP\n");
        checkFrame(regs);
        printf("Debug:\n");

        printf("eax\tebx\tecx\tedx\n%X\t%X\t%X\t%X\n", regs->eax, regs->ebx, regs->ecx, regs->edx);
        printf("\nds\n%X\n", regs->ds);
        printf("\nedi\tesi\tebp\tesp\n%X\t%X\t%X\t%X\n", regs->edi, regs->esi, regs->ebp, regs->esp);
        printf("\neip\tcs\teflags\tuseresp\tss\n%X\t%X\t%X\t%X\t%X\n", regs->eip, regs->cs, regs->eflags, regs->procesp, regs->ss);
        printf("\nerr_code\tfunc_ptr\n%X\t%X\n", regs->errCode, regs->funcPtr);
        printf("\n\nCurrent:\n");
        printf("CS\tDS\tSS\tESP\n%X\t%X\t%X\t%X\n", getCS(), getDS(), getSS(), getESP());
}

void cOverflow(isrVal_t* regs)
{
        do_interrupt(4);
        printf("OF\n");
        checkFrame(regs);
        panic("Overflow");
}

void cBound(isrVal_t* regs)
{
        do_interrupt(5);
        printf("BD\n");
        checkFrame(regs);
        panic("Bounds");
}

void cInvalOp(isrVal_t* regs)
{
        do_interrupt(6);
        printf("IV\n");
        checkFrame(regs);
        cBreakp(regs);
        panic("Invalid Opcode!");
}

void cNoMath(isrVal_t* regs)
{
        do_interrupt(7);
        printf("NM\n");
        checkFrame(regs);
        panic("No math coprocessor");
}

void cDoubleFault(isrVal_t* regs)
{
        do_interrupt(8);
        printf("DF\n");
        checkFrame(regs);
        panic("Double fault");
}

void cDepricated(isrVal_t* regs)
{
        do_interrupt(1);
        do_interrupt(9);
        do_interrupt(15);

        checkFrame(regs);
}

void cInvalidTSS(isrVal_t* regs)
{
        do_interrupt(10);
        printf("IT\n");
        checkFrame(regs);
        panic("Invalid TSS");
}

void cSnp(isrVal_t* regs)
{
        do_interrupt(11);
        printf("FF\n");
        checkFrame(regs);
        panic("Stack not present!");
}
void cStackFault(isrVal_t* regs)
{
        do_interrupt(12);
        printf("SF\n");
        checkFrame(regs);
        panic("Stack fault!");
}

void cGenProt(isrVal_t* regs)
{
        do_interrupt(13);
        printf("GP\n");
        printf("\nGeneral Protection Fault\neip\t\tcs\tds\teflags\tprocesp\t\tss\n");
        printf("%X\t%X\t%X\t%X\t%X\t%X\n", regs->eip, regs->cs & 0xFFFF, regs->ds & 0xFFFF, regs->eflags,
                                                        regs->procesp, regs->ss & 0xFFFF);

        printf("error code: %X\n", regs->errCode);
        printf("\nCurrent:\n");
        printf("CS\tDS\tSS\tESP\n");
        printf("%X\t%X\t%X\t%X\n", getCS(), getDS(), getSS(), getESP());
        checkFrame(regs);
        panic("General Protection fault");
}

void cFpu(isrVal_t* regs)
{
        do_interrupt(16);
        printf("FP\n");
        checkFrame(regs);
        panic("Floating point exception");
}

void cAlligned(isrVal_t* regs)
{
        do_interrupt(17);
        printf("AL\n");
        checkFrame(regs);
        panic("Alligned exception");
}

void cMachine(isrVal_t* regs) {
        do_interrupt(18);
        printf("MACHINE FAULT\n");
        checkFrame(regs);
        panic("MACHINE FAULT!");
}

void cSimd(isrVal_t* regs)
{
        do_interrupt(19);
        printf("SIMD\n");
        checkFrame(regs);
        panic("SSE exception");
}

int cSyscall(isrVal_t* regs)
{
        do_interrupt(80);
        if (sc_list[(uint16_t)regs->eax].syscall != NULL)  {
                if (sc_list[(uint16_t)regs->eax].cpl <= -4) {
                        /**
                         * \todo Replace -4 by current task privilege check
                         */
                        return -E_NORIGHTS;
                }
                return sc_list[(uint16_t)regs->eax].syscall(regs->ebx, regs->ecx, regs->edx);
        }
        return -E_NOT_FOUND;
}
