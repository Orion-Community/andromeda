/*
 *   The openLoader project - Intel (/AMD) CPU features.
 *   Copyright (C) 2011  Michel Megens
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <arch/x86/cpu.h>

#include "mm/heap.h"

ol_lock_t lock = 0;

int
ol_cpuid_available(ol_cpu_t cpu)
{
        uint32_t flags;
        uint32_t flags2;

        cpu->lock(&lock);
        flags = ol_get_eflags();
        ol_set_eflags(flags^OL_CPUID_TEST_BIT);

        flags2 = ol_get_eflags();
        ol_set_eflags(flags); /* restore flags */
        
        if((flags2>>21)&1)
        {
                cpu->unlock(&lock);
                return 0;
        }
        else
        {
                cpu->unlock(&lock);
                return 1;
        }
}

ol_gen_registers_t 
ol_cpuid(uint32_t func)
{
        ol_gen_registers_t regs = kalloc(sizeof(*regs));
        regs->eax = func;
        regs->ecx = 0;
        regs->edx = 0;
        regs->ebx = 0;
        ol_gen_registers_t ret = __ol_cpuid(regs);
        free(regs);
        
        return ret;
}

void
ol_set_eflags(uint32_t flags)
{
        __asm__ __volatile__("movl %0, %%eax \n\t"
                        "pushl %%eax \n\t"
                        "popfl"
                        : /* no output */
                        : "r" (flags) 
                        : "%eax");    
}

uint32_t
ol_get_eflags(void)
{
        uint32_t ret;
        asm volatile("pushfl \n\t"
                        "popl %%eax \n\t"
                        "movl %%eax, %0"
                        : "=r" (ret)
                        : /* no input */
                        : "%eax" /* eax is clobbered */);
        return ret;
}

void
ol_mutex_lock(ol_lock_t *lock)
{
        asm volatile("movb $1, %%al \n\t"
                "l3: xchgb %%al, (%0) \n\t"
                "testb %%al, %%al \n\t"
                "jnz l3"
                : /* no output */
                : "r" (lock)
                : "%eax");
}

void
ol_mutex_release(ol_lock_t *lock)
{
        asm volatile("movb $0, (%0)"
                : /* no output */
                : "r" (lock)
                : "%eax");
}

void
ol_cpu_init(ol_cpu_t cpu)
{
        cpu->flags = 0;
        cpu->lock = &ol_mutex_lock;
        cpu->unlock = &ol_mutex_release;
        cpu->flags |= ol_cpuid_available(cpu) ? 0 : 1;
}

static ol_gen_registers_t
__ol_cpuid(volatile ol_gen_registers_t regs)
{
        __asm__ __volatile__("movl (%1), %%eax \n\t"
                                "movl 4(%1), %%ebx \n\t"
                                "movl 8(%1), %%ecx \n\t"
                                "movl 12(%1), %%edx \n\t"
                                "cpuid \n\t"
                                "movl %%eax, (%0) \n\t"
                                "movl %%ebx, 4(%0) \n\t"
                                "movl %%ecx, 8(%0) \n\t"
                                "movl %%edx, 12(%0) \n\t"
                                : "=r" (regs) /* output in register */
                                : "r" (regs)
                                : "%eax", "%ecx", "%edx", "%ebx" /*
                                                                  * clobbered regs
                                                                  */);
        return regs;
}
