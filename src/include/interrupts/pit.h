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

#ifndef PIT_H
#define	PIT_H

#ifdef	__cplusplus
extern "C"
{
#endif
        
/* some PIT data definitions */
#define OL_RELOAD_DIVISOR 3579545
#define OL_PIT_MAX_FREQ 1193181
#define OL_PIT_MIN_FREQ 18
#define OL_RELOAD_DIVISOR 3579545

/* I/O port definitions */
#define OL_PIT_COMMAND 0x43
#define OL_PIT_CHAN0_DATA 0x40

        typedef uint16_t ol_pit_port_t;
        typedef uint32_t ol_pit_reload_val_t;
        
        typedef struct ol_pit
        {
                uint8_t channel, mode, mask;
                ol_pit_reload_val_t reload_value;
                double timer;
                ol_pit_port_t dport,cport;
        } *ol_pit_system_timer;
        
        int
        ol_pit_init(uint16_t);
        
        static void
        ol_pit_calculate_freq(ol_pit_reload_val_t);
        
        static void
        ol_pit_program_pit(ol_pit_reload_val_t);
        
        static ol_pit_reload_val_t
        ol_pit_calculate_reload(uint16_t);
        
#ifdef	__cplusplus
}
#endif

#endif	/* PIT_H */

