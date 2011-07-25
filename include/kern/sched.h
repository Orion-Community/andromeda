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

#ifndef __KERN_SCHED_H
#define __KERN_SCHED_H
#include <fs/fs.h>
void sched();
void fork (int);
void kill (int);

#define _STATE_RUNNING 0
#define _STATE_WAITING 1
#define _STATE_PAUSING 2
#define _STATE_ZOMBIE  3

struct __REGISTERS
{
  unsigned long rax, rbx, rcx, rdx;
  unsigned long ds, cs, ss, rip;
  unsigned long rsp;
};

struct __TASK_STRUCT
{
  unsigned int pid; // Proccess ID
  unsigned int uid; // User ID

  struct __REGISTERS *registers; // Reference to the registers to be restored
  struct __FS_INODE *procData; // Pointer to /proc/pid

  unsigned int ring; // Privilege level
  char *path; // Path to binary (to look up new data)
  unsigned int priority; // Priority level
  unsigned int spent; // Ammount of time spent in last epoch

  unsigned int state;
};
#endif
