/*
 *   The openLoader project - Programmable Interrupt Controller
 *   Copyright (C) 2011  Michel Megens
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <interrupts/pit.h>
#include <stdlib.h>

static ol_system_pit pit_chan0 = NULL;

void
ol_pit_init(uint16_t hz)
{
        if(pit_chan0 != NULL)
                pit_chan0 = kalloc(sizeof(struct ol_pit));
        ol_pit_reload_val rv;
        
        if(hz <= OL_PIT_MIN_FREQ)
        {
                ol_pit_calculate_freq(0xffff); // slowest reload value
        }
        
        else if(hz >= OL_PIT_MAX_FREQ)
                ol_pit_calculate_freq(1); // fastest reload value
        
        else 
        {
                rv = ol_pit_calculate_reload(hz);
                ol_pit_calculate_freq(rv);
        }
}

static ol_pit_reload_val
ol_pit_calculate_reload(uint16_t hz)
{
        
}
