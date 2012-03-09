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
 * If regs is a pointer to the argument provided to the ISR, this will store the
 * registers as they existed at the time of interrupting for later rescheduling.
 */
int save_task(old_thread, new_thread)
struct thread_state* old_thread;
struct thread_state* new_thread;
{
        if (old_thread == NULL || new_thread == NULL)
                return -E_NULL_PTR;

        /** Move the register to threads stack pointer */
        __asm__ ("mov %%esp, %0"
                : "=r" (old_thread->stack)
        );

        /** Save floats here */

        /** Switch memory segments here!!! */

        return -E_SUCCESS;
}

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

        /** Restore floats here */
       struct task *old = get_current_task();
       save_task(old, task);
       set_current_task(task);
       struct thread_state* thread = task->threads->thread[task->current_thread];

        /** Move the threads stack pointer to register */
        __asm__ ("mov %0, %%esp"
                :
                : "r" (thread->stack)
                : "esp"
        );

        x86_page_set_list(task->pglist);

        return -E_SUCCESS;
}