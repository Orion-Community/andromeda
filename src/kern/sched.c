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

struct __TASK_BRANCH_NODE *task_stack = NULL;
struct __TASK_STATE *idle_stack = NULL;
struct __TASK_STATE *waiting_stack = NULL;
struct __TASK_STATE *current_quantum = NULL;
struct __TASK_STATE *current = NULL;

mutex_t sched_lock;

struct __TASK_STATE* find_process(uint16_t pid)
{
  
}

int task_setup_tree()
{
  return -E_SUCCESS;
}

void get_new_quantum()
{
  panic("No tasks to build quantum");
}

void sched()
{
  panic("Scheduling not supported!");
}

void
thread_cleanup_all(task)
struct __TASK_STATE* task;
{
  struct __THREAD_LIST *itterator = task->threads;
  int i;
  for (; itterator != NULL; itterator = itterator->next)
  {
    for (i=0; i < STD_LIST_SIZE; i++)
    {
      if (itterator->thread[i] == NULL)
        continue;
      free (itterator->thread[i]);
    }
  }
  struct __THREAD_LIST *tmp;
  for (itterator = task->threads; itterator != NULL; itterator = tmp)
  {
    tmp = itterator->next;
    free (itterator);
  }
}

static int
thread_copy(src, dest)
struct __TASK_STATE* src;
struct __TASK_STATE* dest;
{
  struct __THREAD_LIST *carriage = src->threads;
  if (carriage == NULL)
    return -E_NULL_PTR;

  struct __THREAD_LIST *carriage2 = kalloc(sizeof(struct __THREAD_LIST));
  if (carriage2 == NULL)
    return -E_NULL_PTR;
  dest->threads = carriage2;

  int idx = 0;
  for (; carriage != NULL; carriage = carriage->next, carriage2=carriage2->next)
  {
    for (idx = 0; idx < STD_LIST_SIZE; idx++)
    {
      if (carriage->thread[idx] == NULL)
        continue;

      carriage2->thread[idx] = kalloc(sizeof(struct __THREAD_STATE));
      if (carriage2->thread[idx] == NULL)
        return -E_NULL_PTR;
      memcpy(carriage2->thread[idx],
                                                          carriage->thread[idx],
                                                 sizeof(struct __THREAD_STATE));
    }

    if (carriage->next != NULL)
    {
      carriage2->next = kalloc(sizeof(struct __THREAD_LIST));
      if (carriage2->next == NULL)
        return -E_NULL_PTR;
    }
  }

  return -E_SUCCESS;
}

void fork ()
{
  struct __TASK_STATE *new = kalloc(sizeof(struct __TASK_STATE));
  if (new == NULL)
    goto err;

  memcpy (new, current, sizeof (struct __TASK_STATE));
  if (thread_copy(current, new) != -E_SUCCESS)
  {
    thread_cleanup_all(new);
    free(new);
    goto err;
  }

  panic("Could not assign a new process ID");

  return;

err:
  panic("No more space for tasks!"); // Will do for now
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
  struct __THREAD_STATE *thread = kalloc(sizeof(struct __THREAD_STATE));
  struct __THREAD_LIST *list = kalloc(sizeof(struct __THREAD_LIST));
  if (thread == NULL || current == NULL || list == NULL)
    goto err;

  memset(current, 0, sizeof(struct __TASK_STATE));
  memset(thread, 0, sizeof(struct __THREAD_STATE));
  memset(list, 0, sizeof(struct __THREAD_LIST));

  current->threads = list;
  list->thread[0] = thread;
  task_stack = current;

  current->code = &higherhalf;
  current->code_size = ((addr_t)&rodata - (addr_t)&higherhalf);
  current->data = &higherhalf;
  current->data_size = 0 - (addr_t)&rodata;

  thread->stack = stack;
  thread->stack_size = STD_STACK_SIZE;

  printf("WARNING! No path to kernel binary!\n");
  current->path_to_bin = NULL;

  return -E_SUCCESS;

err:
  panic("Could not initialise task administration");
}
