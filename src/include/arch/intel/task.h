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

struct __PROC_MEM_MAP
{
  // Do some memory mapping for the process here ...
};

struct __THREAD_STATE
{
  uint64_t program_counter;
  uint64_t stack_pointer;
  uint64_t base_pointer;
  uint64_t destination_index;
  uint64_t source_index;
  uint64_t ax, bx, cx, dx;
  uint64_t flags;

  struct __THREAD_STATE *next;
  struct __THREAD_STATE *prev;
};

struct __TASK_STATE
{
  struct __THREAD_STATE *threads;
  uint64_t cr0, cr1, cr2, cr3;
  uint16_t cs, ss, ds;

  uint8_t Privilege;

  struct __PROC_MEM_MAP *process_map;
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