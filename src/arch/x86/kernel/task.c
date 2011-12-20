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

#include <Andromeda/sched.h>
#include <Andromeda/error.h>

/**
 * If regs is a pointer to the argument provided to the ISR, this will store the
 * registers as they existed at the time of interrupting for later rescheduling.
 */
int save_task(thread)
struct __THREAD_STATE *thread;
{
        if (thread == NULL)
                return -E_NULL_PTR;

        /** Move the register to threads stack pointer */
        __asm__ ("mov %%esp, %0"
                : "=r" (thread->stack)
        );

        /** Save floats here */

        /** Save memory segments here */

        return -E_SUCCESS;
}

/**
 * If regs is a pointer to the argument offered to the ISR, it will actually
 * perform a context switch (lacking only floating point registers and memory
 * protection
 */
int load_task(thread)
struct __THREAD_STATE *thread;
{
        if (thread == NULL)
                return -E_NULL_PTR;

        /** Restore memory segments here */

        /** Restore floats here */

        /** Move the threads stack pointer to register */
        __asm__ ("mov %0, %%esp"
                :
                : "r" (thread->stack)
                : "esp"
        );

        return -E_SUCCESS;
}