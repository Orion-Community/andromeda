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
#include <arch/x86/task.h>

#ifndef __KERN_SCHED_H
#define __KERN_SCHED_H

#ifdef __cplusplus
extern "C" {
#endif
/** STD_STACK_SIZE speaks for itself */
#define STD_STACK_SIZE 0x8000

/** Defines the standard size of a list element */
#define STD_LIST_SIZE 0x10

/** Defines the standard size of a task list element */
#define TASK_LIST_SIZE 0x100

/**
 * tast_list_type is used to note down the type of __TASK_BRANCH_NODE.
 * It fulfills the task of typeof in OO languages
 */
enum task_list_type
{
	task_list,
	branch_list
};

enum task_state
{
	runnable,
	waiting,
	io_waiting,
	dead,
	zombie
};

/**
 * This structure actually holds the state of the current thread
 */
struct __THREAD_STATE
{
	void* stack;

	void* ss;
	addr_t ss_size;
};

/**
 * It's kinda handy to keep track of which threads you're running
 * This is done by keeping a list of lists of thread states. Can you still keep
 * up?
 */
struct __THREAD_LIST
{
	struct __THREAD_STATE *thread[STD_LIST_SIZE];

	struct __THREAD_LIST *next;
	struct __THREAD_LIST *prev;
};

/**
 * This keeps track of the actual task itself.
 */
struct __TASK_STATE
{
	/** Keep track of threads and task level registers */
	struct __THREAD_LIST *threads;
	struct __PROC_REGS regs;

	/** What state are we in */
	enum task_state state;

	/** Who's your daddy? */
	uint16_t parent_id;

	/** speaks for itself */
	uint8_t priority;
	uint8_t ring_level;

	/** Doubt this is the right implementation, will do for now */
	uint16_t signal;

	/** We're keeping track of how much time you used. It needs to be fair! */
	uint32_t time_used;
	uint32_t time_granted;

	/** Where can we find more code if we swapped some out? */
	char *path_to_bin;

	/** Which physical pages are you using? Probbably going down for rewrite. */
	uint64_t phys_map_idx;

	/** Where are your code and data segments starting? */
	void *code;
	void *data;

	/** How large are those segments? */
	addr_t code_size;
	addr_t data_size;
};

/** Structure for keeping track of threads in the shape of a tree. */
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

/** Some things that might need sharing in the future */
extern struct __TASK_BRANCH_NODE        *task_stack;

/** Some nice functions for you to call ... */
void sched();    /** Go to the next task */
int fork  ();    /** Copy the current task to a new one */
void sig  (int); /** Send a signal to the current task */
void kill (int);

int task_init(); /** Can we please initialise some administration? */

void print_task_stack(); /** Can you show me a proccess dump? */

struct __TASK_STATE* find_task(uint32_t pid);

#ifdef __cplusplus
}
#endif

#endif
