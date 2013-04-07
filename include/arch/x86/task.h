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

#ifndef __ARCH_X86_TASK_H
#define __ARCH_X86_TASK_H

#ifdef __cplusplus
extern "C" {
#endif

struct prog_regs
{
        uint64_t cr0, cr1, cr2, cr3;
        uint16_t cs, ss, ds;
};

/**
typedef struct general_registers
{
        unsigned long eax, ebx, ecx, edx;
        unsigned long edi, esi;
        unsigned long esp, ebp;
} REGS;
*/

struct isr_regs
{
        uint32_t ds;
        uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
        uint32_t funcPtr, errCode;
        uint32_t eip, cs, eflags, procesp, ss;
} __attribute__((packed));
typedef struct isr_regs isrVal_t;

#ifdef __cplusplus
}
#endif

#endif