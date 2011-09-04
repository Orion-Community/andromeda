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

#include <mm/heap.h>
#include <mm/memory.h>
#include <textio.h>

ol_lock_t lock = 0;
ol_cpu_mp_fps_t mp = NULL;

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

        if ((flags2 >> 21)&1)
        {
                cpu->unlock(&lock);
                return 0;
        } else
        {
                cpu->unlock(&lock);
                return 1;
        }
}

ol_gen_registers_t
ol_cpuid(uint32_t func)
{
        ol_gen_registers_t regs = kalloc(sizeof (*regs));
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

        if (cpu->flags & 0x1)
        {
                ol_gen_registers_t regs = ol_cpuid(0);

                if (regs->ebx == 0x756e6547 || regs->ebx == 0x756e6567)
                        cpu->vendor = "INTEL";
                else if (regs->ebx == 0x96444D41 || regs->ebx == 0x68747541)
                        cpu->vendor = "AMD";

                else cpu->vendor = "UNKNOWN";
        }
}

static ol_gen_registers_t
__ol_cpuid(volatile ol_gen_registers_t regs)
{
        static struct ol_gen_regs ret;
        __asm__ __volatile__("movl (%4), %%eax \n\t"
                "movl 4(%4), %%ebx \n\t"
                "movl 8(%4), %%ecx \n\t"
                "movl 12(%4), %%edx \n\t"
                "cpuid"

                : "=a" (ret.eax), /* output in register */
                "=b" (ret.ebx),
                "=c" (ret.ecx),
                "=d" (ret.edx)
                : "r" (regs)
                :);
        return &ret;
}

void
ol_cpu_search_signature(void* mem, uint32_t c)
{
        int i, j = 0;
        for (i = 0; i < c; i++, mem += 16)
        {
                if (!memcmp(mem, "RSD PTR ", strlen("RSD PTR ")) || !memcmp(
                        mem, "_MP_", strlen("_MP_")) || !memcmp(mem, "_SM_", 
                        strlen("_SM_")))
                {
                        ol_validate_table((char*)mem);
                }
        }
}

void
ol_get_system_tables()
{       /* get the ebda pointer */
        uint16_t ebda = *((uint16_t*) ((uint32_t) (0x040E)));
        uint16_t len = ((ebda << 4) + 0x400)-(ebda << 4);

        /* search */
        ol_cpu_search_signature((void*)(ebda<<4), len);
        ol_cpu_search_signature((void*)0x9fc00, 0x400);
        ol_cpu_search_signature((void*)0xe0000, 0x10000);
}

static int
ol_validate_table(char* table)
{
        int i;
        uint8_t checksum = 0, length;
        if(!memcmp(table, "_MP_", 4))
        {
                length = *(table+8)*16;
                for(i = 0; i < length; i++)
                {
                        checksum += *(table+i);
                }
                if(!checksum)
                        mp = (ol_cpu_mp_fps_t)table;
        }
        else if(!memcmp(table, "_SM_", 4))
        {           
                length = *(table+5);
                for(i = 0; i < length; i++)
                {
                        checksum += *(table+i);
                }
                if(!checksum)
                        putc('c');
        }
        else if(!memcmp(table, "RSD PTR ", 8))
        {
                length = 20;
                for(i = 0; i < length; i++)
                {
                        checksum += *(table+i);
                }
                if(!checksum)
                        putc('b');
        }
        
        return checksum;
}
