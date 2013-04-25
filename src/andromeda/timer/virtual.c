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
#include <andromeda/system.h>

static struct virtual_timer head;

static inline struct virtual_timer *
get_virtual_timer_head()
{
        return &head;
}

#if 0
THREAD(virtual_ktimer, timer_data)
{
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
#endif

int destroy_virt_timer(struct virtual_timer* timer)
{
        if(timer->previous)
        {
                timer->previous->next = timer->next;
                if(timer->next)
                        timer->next->previous = timer->previous;
                kfree(timer);
        }
        else if(timer == get_virtual_timer_head())
        {
                /* we are at the beginning of the list */
                memset(timer, 0, sizeof(*timer));
        }
        return -E_SUCCESS;
}

static int virt_register_timer(VIRT_TIMER *timer)
{
        VIRT_TIMER *head = get_virtual_timer_head(), *tmp, *carriage;
        for_each_ll_entry_safe(head, carriage, tmp)
        {
                if(NULL == carriage->next)
                {
                        carriage->next = timer;
                        timer->next = NULL;
                        timer->previous = carriage;
                        break;
                }
        }
        return -E_SUCCESS;
}

int
create_virtual_timer(unsigned int frequency, TIMER *hw, timer_tick_t handle)
{
        VIRT_TIMER *timer = kmalloc(sizeof(*timer));
        if(NULL == timer)
                return -E_NULL_PTR;
        memset(timer, 0, sizeof(*timer));
        timer->frq = frequency;
        timer->hwtimer = hw;
        timer->handle = handle;
        timer->active = TIMER_ACTIVE;
        if(!virt_register_timer(timer))
                return -E_SUCCESS;
        else
                return -E_GENERIC;
}
