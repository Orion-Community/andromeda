/*
 *   The openLoader project - Intel (/AMD) CPU features.
 *   Copyright (C) 2011  Michel Megens
 *   Copyright (C) 2014 2015  Bart Kuivenhoven
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
#include <andromeda/system.h>
#include <andromeda/sched.h>

#include <mm/heap.h>
#include <mm/memory.h>
#include <text.h>

static void __cpuid(volatile struct x86_gen_regs* regs);
static uint64_t __read_msr(uint32_t msr);
static void __write_msr(uint32_t msr, uint64_t value);

extern void* stack;

mutex_t cpu_lock = 0;
volatile ol_cpu_t cpus;
uint8_t cpu_num = 0;

struct x86_gen_regs* x86_cpuid(uint32_t func, struct x86_gen_regs* regs)
{
        if (regs == NULL) {
                return NULL;
        }
        regs->eax = func;
        regs->ecx = 0;
        regs->edx = 0;
        regs->ebx = 0;
        __cpuid(regs);
        return regs;
}

void x86_cpu_init(ol_cpu_t cpu)
{
        cpu->flags = 0;
        cpu->lock = mutex_lock;
        cpu->unlock = mutex_unlock;
        cpu->flags |= x86_cpuid_available() ? 0 : 1;
        cpu->lock(&cpu_lock);
        if (cpu->flags & 0x1) {
                struct x86_gen_regs regs;
                x86_cpuid(0, &regs);

                uint32_t vendor =  x86_get_vendor();
                if (vendor == X86_CPUID_VENDOR_INTEL) {
                        cpu->vendor = "INTEL";
                } else if (vendor == X86_CPUID_VENDOR_AMD) {
                        cpu->vendor = "AMD";
                } else {
                        cpu->vendor = "UNKNOWN";
                }

                x86_cpuid(0x80000000, &regs); /*check the amount of extended functions*/
                if ((regs.eax & 0xff) >= 0x8) { /*if function 0x8 is supported*/
                        x86_cpuid(0x80000008, &regs);
                        cpu->bus_width = regs.eax & 0xff;
                } else {
                        cpu->bus_width = 36; /*default bus width*/
                }

#ifdef __CPU_DBG
                printf("CPU bus width: %i\n", cpu->bus_width);
#endif
        }
        cpus = cpu;
        // cpu_num = cpu_get_num();
        cpu->unlock(&cpu_lock);
        return;
}

static void __cpuid(volatile struct x86_gen_regs* regs)
{
        __asm__ __volatile__("movl (%4), %%eax \n\t"
                        "movl 4(%4), %%ebx \n\t"
                        "movl 8(%4), %%ecx \n\t"
                        "movl 12(%4), %%edx \n\t"
                        "cpuid"

                        : "=a" (regs->eax), /* output in register */
                        "=b" (regs->ebx),
                        "=c" (regs->ecx),
                        "=d" (regs->edx)
                        : "r" (regs)
        );
}

uint64_t cpu_read_msr(uint32_t msr)
{
        return __read_msr(msr);
}

void cpu_write_msr(uint32_t msr, uint64_t val)
{
        __write_msr(msr, val);
}

static uint64_t __read_msr(uint32_t msr)
{
        uint32_t eax, edx;
        __asm__ __volatile__("RDMSR"
                        : "=d" (edx),
                        "=a" (eax)
                        : "c" (msr)
        );
        return (((uint64_t) eax) | edx); /*value is returned in eax:edx*/
}

static void __write_msr(uint32_t msr, uint64_t value)
{
        uint32_t eax = value & 0xffffffff, edx = value >> 32;
        __asm__ __volatile__("WRMSR"
                        :
                        : "c" (msr),
                        "a" (eax),
                        "d" (edx)
        );
}

int enableInterrupts()
{
        asm ("sti");
        return 0;
}

void stack_dump(uint32_t* esp, uint32_t len)
{
        uint32_t i = 0;
        uint32_t cnt = 0;
        uint32_t stack_seg = (uint32_t) &stack;
        uint32_t stack_end = stack_seg + STD_STACK_SIZE;
        int sf = 0;

        printf("Stack trace: \n");
        for (; cnt < len; i++) {
                if ((uint32_t) &esp[i] <= stack_seg
                                || (uint32_t) &esp[i] > stack_end) {
                        /* We just left the stack segment */
                        break;
                }
                if (esp[i] < (uint32_t) &rodata
                                && esp[i] >= (uint32_t) &higherhalf
                                && sf == 1) {
                        /* Print the label */
                        printf("ret %i: %X \tesp: %X\n", cnt, esp[i], &esp[i]);
                        cnt++;
                        sf = 0;
                        /** \todo Try to find the appropriate label and print */
                } else if (esp[i] >= stack_seg && esp[i] <= stack_end) {
                        /* We found a stack frame, next up, a return value */
                        sf = 1;
                } else {
                        /*
                         * We found a local variable or an argument, do nothing
                         */
                }
        }
}
