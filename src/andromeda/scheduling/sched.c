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

/**
 * \fn sched_switch_epoch()
 * \brief Switch to the next epoch.
 * \return Returns the next epoch head.
 * \warning UNTESTED!
 * 
 * This function will reset and resort the current epoch and create a new one.
 */
static struct task *sched_switch_epoch()
{
        /*
         * first of all we sort the current epoch back
         */
        struct task *carriage = task_stack[SCHED_GRAB_BOX].head, *tmp = NULL;
        
        task_stack[SCHED_REALTIME_LIST].tail->next = NULL;
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
         * Move the entire runnable task stack one down.
         */
        uint8_t i = 2; /* entry 0 is the real time list and 1 doesn't need to 
                          be moved */
        for(; i < SCHED_PRIO_SIZE; i++)
        {
                struct task *this = task_stack[i].head;
                struct task *prev = task_stack[i-1].tail;
                
                if(NULL == prev->next)
                {
                        prev->next = this;
                        this->next = NULL;
                }
                else
                        return NULL; /* ll error */
        }
        
        sched_starvation_watchdog();
        
        /*
         * Create the new runnable epoch
         * TODO: Make it work with max epoch size (SCHED_EPOCH_SIZE).
         */
        struct task *epoch = task_stack[SCHED_REALTIME_LIST].tail;
        if(epoch->next != NULL)
                return NULL;
        else
        {
                epoch->next = task_stack[SCHED_GRAB_BOX].head;
                task_stack[SCHED_GRAB_BOX].head->next = NULL;
                return task_stack[SCHED_REALTIME_LIST].head;
        }
        
        return NULL; // shouldn't get here, if it does, complain.
}

/**
 * \fn sched_starvation_watchdog()
 * \brief Checks if there is no starvation.
 * \TODO: Write the actual function.
 * 
 * This function will set tasks temporarily to a lower priority if there is
 * starvation.
 */
static int sched_starvation_watchdog()
{
        return -E_NOFUNCTION;
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

