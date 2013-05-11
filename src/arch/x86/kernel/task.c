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

#include <andromeda/task.h>
#include <andromeda/sched.h>
#include <andromeda/error.h>
#include <mm/paging.h>

/**
 * \fn load_task(TASK_STATE *task)
 * \brief Switch to another <i>task</i>.
 * \param task New task to which has to be loaded.
 * \return Error code. See <i>error.h</i> for more information.
 *
 * This function loads a new task and starts the execution.
 */
int context_switch(task)
struct task *task;
{
        if (task == NULL)
                return -E_NULL_PTR;

        /* Get the old task */
        struct task *old = get_current_task();
        /* Mark the new task as running */
        set_current_task(task);
        /* retrieve thread data for easier inline assembly */
        struct thread_state* old_t = old->threads->thread[old->current_thread];
        struct thread_state* thrd = task->threads->thread[task->current_thread];

        /*
         * Swap the virtual memory.
         * If al goes wel the kernel space won't change.
         */
        //x86_page_set_list(task); -> refers to the old system, has been removed
        vm_load_task(); // -> refers to the new system. Still to be implemented

        /** \todo push floating point registers and push pointer */

        /*
         * pusha        Push all registers to stack
         * mov esp, %0  Move the old stack pointer to data structure
         * mov %1, esp  Move the new stack pointer from data structure
         * popa         Pop all (the new) registers from stack
         */
        __asm__ ("pusha\n\t"
                "mov %%esp, %0\n\t"
                "mov %1, %%esp\n\t"
                "popa\n\t"
                : "=r" (old_t->stack)
                : "r" (thrd->stack)
        );

        /** \todo pop pointer and pop floating point registers */

        /* We should now have switched tasks */
        return -E_SUCCESS;
}
