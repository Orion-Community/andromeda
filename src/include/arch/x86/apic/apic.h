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

#ifndef __APIC_H
#define	__APIC_H

#ifdef	__cplusplus
extern "C" {
#endif

    typedef struct apic
    {
        
    }*ol_apic_t;
    
    static int 
    ol_detect_apic(ol_cpu_t cpu);
    
    void
    ol_apic_init(ol_cpu_t cpu);

#ifdef	__cplusplus
}
#endif

#endif	/* APIC_H */

