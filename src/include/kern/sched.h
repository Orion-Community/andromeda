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
#include <stdlib.h>
#include <types.h>
void sched();
void fork ();
void kill (int);

struct __kern_sched_page_entry // Page directory entry for a certain process
{
  addr_t index_list;
  struct __kern_sched_page_entry *next;
};

struct __kern_sched_segment
{
  void* base; // Base address
  size_t length;
  struct __kern_sched_page_entry* pagedir; // Page directory for segment
  boolean executable;
  boolean writable;
  boolean swappable;
};

#define _STATE_RUNNING 0
#define _STATE_WAITING 1
#define _STATE_PAUSING 2
#define _STATE_ZOMBIE  3

struct __thread_state
{
  unsigned int tid;      // The id of the specific thread
  isrVal_t* registers;   // The registers as stored by the irq stub
  unsigned int state;    // The state of the current thread
  unsigned int nice; // The thread based priority
  unsigned int used;     // The running time used by the thread
  struct __thread_state *next;
  struct __thread_state *previous;
};

struct __task_struct
{
  unsigned int pid; // Proccess ID
  unsigned int uid; // User ID

  struct __thread_state *threads;
  struct __FS_INODE *proc_data; // Pointer to /proc/pid

  boolean userspace; // false for kernel mode
  char *ptb; // Path to binary (to look up new data)
  char *working_dir;
  unsigned int nice; // Priority level
  unsigned int spent; // Ammount of time spent in current epoch

  struct __kern_sched_segment* text;
  struct __kern_sched_segment* data;
  struct __kern_sched_segment* bss;
  struct __kern_sched_segment* stack;

  struct __task_struct *previous;
  struct __task_struct *next;
};

extern struct __task_struct* task_stack;

int sched_init();
void sched_next_task();
#endif
