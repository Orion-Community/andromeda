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
#include <error/error.h>

/**
 * If regs is a pointer to the argument provided to the ISR, this will store the
 * registers as they existed at the time of interrupting for later rescheduling.
 */
int task_save_regs(regs, thread)
isrVal_t *regs;
struct __THREAD_STATE *thread;
{
	/** Store the general purpose stack */
	thread->regs.rsp = regs->esp;
	/** And let's do the floating point stack */
// 	thread->regs.fsp = regs->fsp;
	return -E_SUCCESS;
}


/**
 * If regs is a pointer to the argument offered to the ISR, it will actually
 * perform a context switch (lacking only floating point registers and memory
 * protection
 */
int task_load_regs(regs, thread)
isrVal_t *regs;
struct __THREAD_STATE *thread;
{
	/** Reset the general purpose stack */
	regs->esp = thread->regs.rsp;
	/** And reset the floating point stack */
// 	regs->fsp = thread->regs.fsp;
	
	return -E_SUCCESS;
}