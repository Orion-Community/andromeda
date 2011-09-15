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

#include <arch/x86/pit.h>
#include <stdlib.h>
#include <sys/io.h>
#include <text.h>

static ol_pit_system_timer_t pit_chan0 = NULL;

int
ol_pit_init(uint32_t hz)
{
        if(pit_chan0 == NULL)
        {
                pit_chan0 = kalloc(sizeof(struct ol_pit));
                
        } else return -1;
        
	pit_chan0->channel = 0;
	pit_chan0->mode = OL_PIT_RATE_GEN;
        pit_chan0->access = 3;
        pit_chan0->cport = OL_PIT_COMMAND;
        pit_chan0->dport = OL_PIT_CHAN0_DATA;
        
        if(hz <= OL_PIT_MIN_FREQ)
        {

                pit_chan0->reload_value = 0xffff;
                ol_pit_calculate_freq(pit_chan0); // slowest reload value
        }
        
        else if(hz >= OL_PIT_MAX_FREQ)
        {
                putc(0x42);
                pit_chan0->reload_value = 1;
                ol_pit_calculate_freq(pit_chan0); // fastest reload value
        }
        else 
        {
                pit_chan0->reload_value = ol_pit_calculate_reload(hz);
                ol_pit_calculate_freq(pit_chan0);           
        }
        ol_pit_program_pit(pit_chan0);
#ifdef __MEMTEST
        free(pit_chan0);
#endif
}

static ol_pit_reload_val_t
ol_pit_calculate_reload(uint16_t hz)
{
        register ol_pit_reload_val_t ret = OL_RELOAD_DIVISOR/hz;
        if(ret < (OL_RELOAD_DIVISOR/2)) ret++;
        ret /= 3;
        if(ret < (3/2)) ret++;
        return ret;
}

static void
ol_pit_calculate_freq(ol_pit_system_timer_t pit)
{
        pit->timer = (pit->reload_value * 0xDBB3A062) << 10;
}

static void
ol_pit_program_pit(ol_pit_system_timer_t pit)
{
        ol_pit_calc_mask(pit);
        outb(pit->cport, pit->mask); /* set the mask */
        outb(pit->dport, (uint8_t)(pit->reload_value)); /* low byte */
        outb(pit->dport, (uint8_t)(pit->reload_value>>8)); /* high byte */
        iowait();
}
