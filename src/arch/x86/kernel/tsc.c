/*
 *  Andromeda - CPU Timer system
 *  Copyright (C) 2012  Michel Megens
 *  Copyright (c) 2014  Bart Kuivenhoven
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
#include <types.h>
#include <arch/x86/timer.h>
#include <andromeda/system.h>

int
__get_cpu_tick_inline(struct cpu_time *time)
{
        unsigned int low, high;
        __asm__ __volatile__("rdtsc\n"
                                : "=a" (low), "=d" (high)
                                );
        time->low = low;
        time->high = high;
        return -E_SUCCESS;
}

uint64_t
get_cpu_tick()
{
        struct cpu_time time;
        __get_cpu_tick_inline(&time);

        /* struct cpu_time *time = kmalloc(sizeof(*time)); */

        uint64_t ret = time.low;
        ret |= ((uint64_t)(time.high)) << 32;
        return ret;
}


