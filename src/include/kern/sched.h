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

#include <stdlib.h>
#include <types.h>
#include <arch/intel/task.h>

#ifndef __KERN_SCHED_H
#define __KERN_SCHED_H

#ifdef __cplusplus
extern "C" {
#endif

#define STD_STACK_SIZE 0x8000

struct __THREAD_STATE
{
  struct __THREAD_REGS regs;
  void* stack;
  addr_t stack_size;

  struct __THREAD_STATE *next;
  struct __THREAD_STATE *prev;
};

struct __TASK_STATE
{
  struct __TASK_STATE *next;
  struct __TASK_STATE *prev;

  struct __THREAD_STATE *threads;
  struct __PROC_REGS regs;

  uint8_t priority;
  uint8_t ring_level;

  uint16_t signal;

  uint32_t time_used;
  uint32_t time_granted;

  uint8_t sched_state;

  char *path_to_bin;

  uint64_t phys_map_idx;

  void *code;
  void *data;

  addr_t code_size;
  addr_t data_size;
};

extern struct __TASK_STATE *task_stack;
extern struct __TASK_STATE *idle_stack;
extern struct __TASK_STATE *waiting_stack;
extern struct __TASK_STATE *current_quantum;
extern struct __TASK_STATE *current;

void sched();
void fork ();
void kill (int);

int sched_init();
void sched_next_task();

#ifdef __cplusplus
}
#endif

#endif
