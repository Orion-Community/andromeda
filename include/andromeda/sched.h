/*
 *  Andromeda
 *  Copyright (C) 2011  Bart Kuivenhoven, Michel Megens
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __KERN_SCHED_H
#define __KERN_SCHED_H

#include <stdlib.h>
#include <types.h>
#include <arch/x86/task.h>
#include <mm/paging.h>

#ifdef __cplusplus
extern "C" {
#endif
/** STD_STACK_SIZE speaks for itself */
#define STD_STACK_SIZE 0x8000

/** Defines the standard size of a list element */
#define SCHED_LIST_SIZE 0x10

/** Defines the standard size of a task list element */
#define TASK_LIST_SIZE 0x100

/*
 * Algorithm defs
 */
#define SCHED_PRIO_SIZE 40
#define SCHED_REALTIME_LIST 0x0
#define SCHED_GRAB_BOX 0x1
#define SCHED_EPOCH_SIZE 0x10

/**
 * tast_list_type is used to note down the type of __TASK_BRANCH_NODE.
 * It fulfills the task of typeof in OO languages
 */
enum task_list_type
{
        task_list,
        branch_list
};

enum task_status
{
        RUNNABLE,
        WAITING,
        IO_WAITING,
        DEAD,
        ZOMBIE
};

/**
 * This structure actually holds the state of the current thread
 */
struct thread_state
{
        void* stack;
        void* ss;
        addr_t ss_size;

        enum task_status state;
};

/**
 * It's kinda handy to keep track of which threads you're running
 * This is done by keeping a list of lists of thread states. Can you still keep
 * up?
 */
struct thread_list
{
        struct thread_state* thread[SCHED_LIST_SIZE];
        enum task_status state;

        struct thread_list* next;
        struct thread_list* prev;
};

/**
 * This keeps track of the actual task itself.
 */
struct task
{
        /** Keep track of threads and task level registers */
        struct thread_list* threads;
        uint8_t current_thread;
        struct prog_regs regs;
        //REGS* registers;

        /** What state are we in */
        enum task_status state;

        /** Who's your daddy? */
        uint16_t parent_id;

        /** speaks for itself */
        uint8_t priority;
        uint8_t ring_level;

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

        /** page list */
        struct mm_page_list *pglist;
};

/** Structure for keeping track of threads in the shape of a tree. */
struct task_branch_node
{
        uint16_t full; /** Bitmap of which entries are full */
        enum task_list_type type;

        struct task_branch_node* parent;
        union {
                struct task_state*       task   [TASK_LIST_SIZE];
                struct task_branch_node* branch [TASK_LIST_SIZE];
        };
};

struct task_head
{
        struct task_head* next;
        struct task_head* prev;

        struct task* task;
};

struct task_list_head
{
        struct task_list_head *next;
        struct task_list_head *prev;

        struct task_head *head;
        struct task_head *tail;
        uint32_t size;
};

/** Some things that might need sharing in the future */
extern struct task_list_head task_stack[SCHED_PRIO_SIZE];
extern struct task *current_task;

static inline struct task*
get_current_task()
{
        return current_task;
}

static inline void
set_current_task(struct task *task)
{
        current_task = task;
}

extern void sched();
extern int fork();    /** Copy the current task to a new one */
extern void sig(int); /** Send a signal to the current task */
extern void kill(int);

int task_init(); /** Can we please initialise some administration? */

void print_task_stack(); /** Can you show me a proccess dump? */

struct task* find_task(uint32_t pid);

#ifdef __cplusplus
}
#endif

#endif
