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
#define STD_NO_CHILDREN 0x10
#define STD_LIST_SIZE 0x10
#define TASK_LIST_SIZE 0x10

/**
 * tast_list_type is used to note down the type of __TASK_BRANCH_NODE.
 * It fulfills the task of typeof in OO languages
 */
enum task_list_type
{
  task_list,
  branch_list
};

struct __THREAD_STATE
{
  struct __THREAD_REGS regs;
  void* stack;
  addr_t stack_size;
};

struct __THREAD_LIST
{
  struct __THREAD_STATE *thread[STD_LIST_SIZE];

  struct __THREAD_LIST *next;
  struct __THREAD_LIST *prev;
};

struct __TASK_STATE
{
  struct __THREAD_LIST *threads;
  struct __PROC_REGS regs;
  
  uint16_t parent_id;

  uint8_t priority;
  uint8_t ring_level;

  uint16_t signal;

  uint32_t time_used;
  uint32_t time_granted;

  char *path_to_bin;

  uint64_t phys_map_idx;

  void *code;
  void *data;

  addr_t code_size;
  addr_t data_size;
};

struct __TASK_BRANCH_NODE
{
  uint16_t full; /** Bitmap of which entries are full */
  enum task_list_type type;

  struct __TASK_BRANCH_NODE* parent;
  union {
    struct __TASK_STATE       *task   [TASK_LIST_SIZE];
    struct __TASK_BRANCH_NODE *branch [TASK_LIST_SIZE];
  };
};

extern struct __TASK_BRANCH_NODE        *task_stack;
extern struct __TASK_STATE              *idle_stack;
extern struct __TASK_STATE              *waiting_stack;
extern struct __TASK_STATE              *current_quantum;
extern struct __TASK_STATE              *current;

void sched();
void fork ();
void kill (int);

int task_init();

#ifdef __cplusplus
}
#endif

#endif
