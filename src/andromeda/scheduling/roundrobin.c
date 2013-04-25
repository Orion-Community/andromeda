/*
 *  Andromeda, Educative Kernel System.
 *  Copyright (C) 2012 Bart Kuivenhoven
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

#include <stdio.h>
#include <stdlib.h>
#include <andromeda/system.h>

#include <thread.h>

/**
 * DO NOT USE THIS SCHEDULER AS IT IS MEANT FOR RESEARCH PURPOSES.
 */

int next_pid = 1;

struct rr_thread {
        struct rr_thread* next;
        void (*run)();
        int signal;
        int pid;
};

struct rr_thread* list_start = NULL;
struct rr_thread* list_end = NULL;

int rr_thread_init(void (*run)())
{
        struct rr_thread* t = kmalloc(sizeof(*t));
        memset(t, 0, sizeof(*t));
        t->next = list_end;
        t->run = run;
        if (list_start == NULL)
        {
                list_start = t;
                list_end = t;
        }
        else
                list_end->next = t;

        return -E_SUCCESS;
}

int rr_sched()
{
        struct rr_thread* t = list_start;
        t->run();
        if (list_start->next != NULL)
        {
                list_start = list_start->next;
                list_end->next = t;
                list_end = t;
                t->next = NULL;
        }
        return -E_SUCCESS;
}

