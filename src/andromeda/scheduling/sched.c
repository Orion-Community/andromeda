/*
    Andromeda, Educative Kernel System.
    Copyright (C) 2012 Michel Megens

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

/** \file */

#include <stdlib.h>
#include <thread.h>
#include "sched.h"

#include <andromeda/sched.h>
#include <andromeda/timer.h>

volatile boolean scheduling = FALSE;

/**
 * \var stack
 * \brief Kernel stack
 */
unsigned char stack[STD_STACK_SIZE];

struct task *current_task = NULL;

/**
 * \var task_stack
 * \brief Task priority queue's.
 *
 * Each priority has its own stack and every run the entire queue is moved one
 * down.
 */
struct task_list_head task_stack[SCHED_PRIO_SIZE], *sched_task_waiting;

void
sched()
{}

int fork()
{
        return -E_NOFUNCTION;
}

void sig(int sig)
{}

void kill(int num)
{}

int task_init()
{
        return -E_NOFUNCTION;
}

