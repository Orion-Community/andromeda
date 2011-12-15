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
	thread->regs.ip = regs->eip;
	thread->regs.bp = regs->ebp;
	thread->regs.di = regs->edi;
	thread->regs.si = regs->esi;
	thread->regs.ax = regs->eax;
	thread->regs.bx = regs->ebx;
	thread->regs.cx = regs->ecx;
	thread->regs.dx = regs->edx;
	thread->regs.sp = regs->procesp;
	thread->regs.flags = regs->eflags;

	/**
	 * Something must be figured out still for floating point registers ...
	 */

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
	regs->eip = thread->regs.ip;
	regs->ebp = thread->regs.bp;
	regs->edi = thread->regs.di;
	regs->esi = thread->regs.si;
	regs->eax = thread->regs.ax;
	regs->ebx = thread->regs.bx;
	regs->ecx = thread->regs.cx;
	regs->edx = thread->regs.dx;
	regs->procesp = thread->regs.sp;
	regs->eflags = thread->regs.flags;

	/**
	 * Here too something has to be figured out for floats ...
	 */
	
	return -E_SUCCESS;
}