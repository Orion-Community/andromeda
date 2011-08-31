/*
 *   The OpenLoader project - Local APIC interface
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

#include <arch/x86/cpu.h>
#include <stdlib.h>

#ifndef __APIC_H
#define	__APIC_H

/* I/O registers */
#define OL_APIC_IMCR_COMMAND 0x22
#define OL_APIC_IMCR_DATA 0x23

/* I/O commands */
#define OL_APIC_IMCR_SELECT 0x70
#define OL_APIC_IMCR_PASSTROUGH_APIC 0x1

/* APIC constants */
#define OL_APIC_BASE_ADDRESS 0x0FEC00000

#ifdef	__cplusplus
extern "C" {
#endif

	typedef uint32_t ol_apic_reg_t;
    typedef struct apic
    {
        const uint8_t id;
		void (*ol_apic_write) (ol_apic_reg_t, uint32_t);
    }*ol_apic_t;
    
    static int 
    ol_detect_apic(ol_cpu_t cpu);

	static void
	ol_apic_set_mode(ol_apic_t apic, uint16_t mode);
    
    void
    ol_apic_init(ol_cpu_t cpu);

#ifdef	__cplusplus
}
#endif

#endif	/* APIC_H */

