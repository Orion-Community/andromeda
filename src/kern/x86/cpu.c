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

int
ol_cpuid_available(ol_cpu_t cpu) {
        uint32_t flags;
        uint32_t flags2;

        cpu->lock(&lock);
        flags = ol_get_eflags();
        ol_set_eflags(flags^OL_CPUID_TEST_BIT);

        flags2 = ol_get_eflags();
        ol_set_eflags(flags); /* restore flags */

        if ((flags2 >> 21)&1) {
                cpu->unlock(&lock);
                return 0;
        } else {
                cpu->unlock(&lock);
                return 1;
        }
}

ol_gen_registers_t
ol_cpuid(uint32_t func) {
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
ol_set_eflags(uint32_t flags) {
        __asm__ __volatile__("movl %0, %%eax \n\t"
                "pushl %%eax \n\t"
                "popfl"
                : /* no output */
                : "r" (flags)
                : "%eax");
}

uint32_t
ol_get_eflags(void) {
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
ol_mutex_lock(ol_lock_t *lock) {
        asm volatile("movb $1, %%al \n\t"
                "l3: xchgb %%al, (%0) \n\t"
                "testb %%al, %%al \n\t"
                "jnz l3"
                : /* no output */
                : "r" (lock)
                : "%eax");
}

void
ol_mutex_release(ol_lock_t *lock) {
        asm volatile("movb $0, (%0)"
                : /* no output */
                : "r" (lock)
                : "%eax");
}

void
ol_cpu_init(ol_cpu_t cpu) {
        cpu->flags = 0;
        cpu->lock = &ol_mutex_lock;
        cpu->unlock = &ol_mutex_release;
        cpu->flags |= ol_cpuid_available(cpu) ? 0 : 1;

        if (cpu->flags & 0x1) {
                ol_gen_registers_t regs = ol_cpuid(0);

                if (regs->ebx == 0x756e6547 || regs->ebx == 0x756e6567)
                        cpu->vendor = "INTEL";
                else if (regs->ebx == 0x96444D41 || regs->ebx == 0x68747541)
                        cpu->vendor = "AMD";

                else cpu->vendor = "UNKNOWN";
        }
}

static ol_gen_registers_t
__ol_cpuid(volatile ol_gen_registers_t regs) {
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

static void *
ol_cpu_mp_search_config_table(char* mem, int count) {
        int length, i;
        unsigned char checksum;
        while (i < count) {
                if (mem[0] == 'R' && mem[1] == 'S' && mem[2] == 'D' &&
                        mem[3] == ' ' && mem[4] == 'P' && mem[5] == 'T' &&
                        mem[6] == 'R' && mem[7] == ' ') {

                        return mem;
                }
                mem += 16;
                i+=16;
        }
        return NULL;
}

int
ol_get_mp_config_header() {
        char * x;
        if ((x = ol_cpu_mp_search_config_table((char*) 0xe0000, 0x100000-0xe0000)) 
                != NULL) {
                uint32_t * y = (uint32_t*)(x+16);
                printnum(*(y), 16, 0,0);
                return 0;
        }

        uint16_t ebda = *((uint16_t*) ((uint32_t) (0x040E)));
        uint16_t len = ((ebda<<4)+0x400)-(ebda<<4);
        if (ol_cpu_mp_search_config_table((char*) (ebda << 4), len)
                != NULL) {
                putc(0x42);
                return 0;
        }
        return 1;
        ;
}

