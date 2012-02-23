/*
 *  Andromeda - Virtual timer system
 *  Copyright (C) 2012 Michel Megens
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

#include <stdlib.h>

#include <andromeda/timer.h>
#include <andromeda/timer/virtual.h>
#include <andromeda/timer.h>

static struct virtual_timer head;

static inline struct virtual_timer *
get_virtual_timer_head()
{
        return &head;
}

THREAD(virtual_ktimer, timer_data)
{
        VIRT_TIMER *timers = (VIRT_TIMER*)timer_data;

        VIRT_TIMER *carriage, *tmp, *head = get_virtual_timer_head();
        for_each_ll_entry_safe(head, carriage, tmp)
        {
                long tick = (long)carriage->hwtimer->tick;
                if((tick % carriage->frq) == 0 && carriage->active)
                {
                        carriage->handle((void*)carriage);
                        if(ONE_SHOT == carriage->mode)
                                destroy_virt_timer(carriage);
                }
        }
}

static int destroy_virt_timer(struct virtual_timer* timer)
{
        if(timer->previous)
        {
                timer->previous->next = timer->next;
                timer->next->previous = timer->previous;
        }
        return -E_NOFUNCTION;
}
