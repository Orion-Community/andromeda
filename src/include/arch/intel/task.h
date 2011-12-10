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

struct __FLOAT_REGISTER
{
  uint64_t hi, lo; // 128-bits
};

struct __THREAD_REGS
{
  uint64_t ip,  bp,  di,  si;  /** IP, base pointer, dest-index, src-index */
  uint64_t ax,  bx,  cx,  dx;  /** General purpose (standard)              */
  uint64_t r8,  r9,  r10, r11; /** General purpose (r8 - r11)              */
  uint64_t r12, r13, r14, r15; /** General purpose (12 - r15)              */

  uint64_t sp;                 /** Stack pointer                           */
  uint64_t flags;              /** Flags register                          */

  struct __FLOAT_REGISTER xmm1,  xmm2,  xmm3,  xmm4; /** Kinda hope this will */
  struct __FLOAT_REGISTER xmm5,  xmm6,  xmm7,  xmm8; /** work ...             */
  struct __FLOAT_REGISTER xmm9,  xmm10, xmm11, xmm12;
  struct __FLOAT_REGISTER xmm13, xmm14, xmm15, xmm16;
};

struct __PROC_REGS
{
  uint64_t cr0, cr1, cr2, cr3;
  uint16_t cs, ss, ds;
};

struct isr_regs
{
  uint16_t ds;
  unsigned long edi, esi, ebp, esp, ebx, edx, ecx, eax;
  unsigned long funcPtr, errCode;
  unsigned long eip, cs, eflags, procesp, ss;
} __attribute__((packed));
typedef struct isr_regs isrVal_t;

#ifdef __cplusplus
}
#endif

#endif