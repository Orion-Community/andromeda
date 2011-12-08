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
#include <kern/sched.h>

volatile boolean scheduling = FALSE;

unsigned char stack[STD_STACK_SIZE];

struct __TASK_STATE *task_stack = NULL;
struct __TASK_STATE *idle_stack = NULL;
struct __TASK_STATE *waiting_stack = NULL;
struct __TASK_STATE *current_quantum = NULL;
struct __TASK_STATE *current = NULL;

void get_new_quantum()
{
  panic("No tasks to build quantum");
}

void sched()
{
  panic("Scheduling not supported!");
}

void fork ()
{
  panic("No forking code");
}

void kill (int signal)
{
  panic("No processes to send signal");
}

int task_init()
{
  if (current != NULL)
    panic("Trying to init scheduling on a running system!");

  current = kalloc(sizeof(struct __TASK_STATE));
  struct __THREAD_STATE *tmp = kalloc(sizeof(struct __THREAD_STATE));
  if (tmp == NULL || current == NULL)
    goto err;

  memset(current, 0, sizeof(struct __TASK_STATE));
  memset(tmp, 0, sizeof(struct __THREAD_STATE));

  current->threads = tmp;
  task_stack = current;

  current->code = &higherhalf;
  current->code_size = ((addr_t)&rodata - (addr_t)&higherhalf);
  current->data = &higherhalf;
  current->data_size = 0 - (addr_t)&rodata;

  tmp->stack = stack;
  tmp->stack_size = STD_STACK_SIZE;

  printf("WARNING! No path to kernel binary!\n");
  current->path_to_bin = NULL;

  return -E_SUCCESS;

err:
  panic("Could not initialise task administration");
}
