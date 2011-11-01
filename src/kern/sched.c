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

#include <kern/sched.h>
#include <error/panic.h>

volatile boolean scheduling = FALSE;
struct __task_struct *task_stack = NULL;
struct __task_struct **current_tasks = NULL;

uint32_t cpus = 0;

struct __task_struct*
sched_init_task(int user_id, boolean userspace, char* path_to_binary, int pid)
{
  struct __task_struct* new_task = kalloc(sizeof(struct __task_struct));
  if(new_task == NULL) return NULL;
  memset (new_task, 0, sizeof(struct __task_struct));

  new_task->uid       = user_id;
  new_task->pid       = pid;
  new_task->userspace = userspace;
  new_task->ptb       = path_to_binary;

  new_task->text = kalloc(sizeof(struct __kern_sched_segment));
  if (new_task->text == NULL)
  {
    free(new_task);
    return NULL;
  }
  new_task->data = kalloc(sizeof(struct __kern_sched_segment));
  if (new_task->data == NULL)
  {
    free(new_task->text); free(new_task);
    return NULL;
  }
  new_task->bss = kalloc(sizeof(struct __kern_sched_segment));
  if (new_task->bss == NULL)
  {
    free(new_task->text); free(new_task->data); free(new_task);
    return NULL;
  }
  new_task->stack = kalloc(sizeof(struct __kern_sched_segment));
  if (new_task->stack == NULL)
  {
    free(new_task->text); free(new_task->data); free(new_task->bss);
    free(new_task);
    return NULL;
  }
  new_task->threads = kalloc(sizeof(struct __thread_state));
  if (new_task->threads == NULL)
  {
    free(new_task->text); free(new_task->data); free(new_task->bss);
    free(new_task->stack); free(new_task);
  }
  new_task->threads->tid = 0;
  new_task->threads->registers = NULL;
  new_task->threads->nice = 0;
  new_task->threads->used = 0;
  new_task->threads->next = NULL;
  new_task->threads->previous = NULL;

  new_task->nice = 0;
  new_task->spent = 0;
  new_task->working_dir = "/";

  return new_task;
}

int sched_init()
{
  struct __task_struct* proc_init = sched_init_task(0, FALSE, "/", 0);

  if (proc_init == NULL)
    panic("No memory for init process");

  uint32_t cpus = /* get_no_cpus() */1;
  if (cpus == 0)
    cpus = 1;
  current_tasks = kalloc(sizeof(struct __task_struct) * cpus);
  if (current_tasks == NULL)
    panic("No memory for the current tasks structure");
  int i = 0;
  for (; i < cpus; i++)
  {
    current_tasks[i] = proc_init;
  }

  return -E_UNFINISHED;
}

int sched_start_switching()
{
  return -E_NOFUNCTION;
}

void sched_next_task()
{
	panic("Could not schedule");
}

void fork()
{
	panic("Fork wasn't implemented");
}

void kill (int pid)
{
	panic("Kill needs implementation");
}
