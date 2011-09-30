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
struct __task_struct* task_stack = NULL;

int sched_init_task(int user_id, boolean userspace, char* path_to_binary)
{
  struct __task_struct* new_task = kalloc(sizeof(struct __task_struct));
  memset (new_task, 0, sizeof(struct __task_struct));

  printf("sizeof new_task: %X\n", sizeof(struct __task_struct));
  
  return -E_UNFINISHED;
}

int sched_init_stack()
{
  sched_init_task(0, 0, NULL);
  return -E_UNFINISHED;
}

void sched()
{
	panic("Could not schedule");
}

void fork(int pid)
{
	panic("Fork wasn't implemented");
}

void kill (int pid)
{
	panic("Kill needs implementation");
}
