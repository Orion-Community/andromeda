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

struct __THREAD_REGS
{
  uint64_t ip,  bp,  di,  si;  /** IP, base pointer, dest-index, src-index */
  uint64_t ax,  bx,  cx,  dx;  /** General purpose (standard)              */
  uint64_t r8,  r9,  r10, r11; /** General purpose (r8 - r11)              */
  uint64_t r12, r13, r14, r15; /** General purpose (12 - r15)              */

  uint64_t sp;                 /** Stack pointer                           */
  uint64_t flags;              /** Flags register                          */

  double xmm1_lo, xmm1_hi;     /** Floating point registers                */
  double xmm2_lo, xmm2_hi;     /** Had to split this up into a low and     */
  double xmm3_lo, xmm3_hi;     /** high part because gcc doesn't support   */
  double xmm4_lo, xmm4_hi;     /** 128 bits floats on 32-bits systems      */
  double xmm5_lo, xmm5_hi;
  double xmm6_lo, xmm6_hi;
  double xmm7_lo, xmm7_hi;
  double xmm8_lo, xmm8_hi;

  double xmm9_lo,  xmm9_hi;    /** Floating point registers                */
  double xmm10_lo, xmm10_hi;
  double xmm11_lo, xmm11_hi;
  double xmm12_lo, xmm12_hi;
  double xmm13_lo, xmm13_hi;
  double xmm14_lo, xmm14_hi;
  double xmm15_lo, xmm15_hi;
  double xmm16_lo, xmm16_hi;
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