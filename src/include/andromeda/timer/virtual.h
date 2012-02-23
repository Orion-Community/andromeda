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

#ifndef __VIRTUAL_TIMER_H
#define __VIRTUAL_TIMER_H

#include <stdlib.h>

#include <andromeda/timer/virtual.h>
#include <andromeda/timer.h>

typedef enum
{
        PERIODIC,
        ONE_SHOT,
} virtual_timer_mode_t;

typedef struct virtual_timer
{
        char *name;
        uint64_t id;

        uint32_t frq;
        uint64_t tick;
        timer_tick_t handle;
        virtual_timer_mode_t mode;
} VIRT_TIMER;

#endif
