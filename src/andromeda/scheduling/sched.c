/*
    Andromeda
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
#include <thread.h>

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

static struct task *sched_switch_epoch()
{
        /*
         * first of all we sort the current epoch back
         */
        struct task *carriage = task_stack[SCHED_GRAB_BOX].head, *tmp = NULL;
        while(carriage)
        {
                struct task *tail = NULL;
                if(carriage->state == WAITING)
                        tail = task_stack[carriage->priority].tail;
                else
                        tail = sched_task_waiting->tail;
                if(tail->next == NULL)
                {
                        tail->next = carriage;
                        tmp = carriage;
                }
                else /* error in linked list */
                        return NULL;
                
                carriage = carriage->next;
                tmp->next = NULL;
                if(carriage->next != NULL)
                        continue;
                else
                        break;
        }
        
        /*
         * TODO: Move the entire runnable task stack one down.
         */
        return NULL;
}

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

