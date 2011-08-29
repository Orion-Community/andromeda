/*
 *   The openLoader project - Intel (/AMD) CPU features.
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

#include <stdlib.h>

#ifndef CPU_H
#define	CPU_H

#ifdef	__cplusplus
extern "C"
{
#endif
#define OL_CPU_INFO_AVAILABLE 0x1

        typedef struct ol_gen_regs
        {
                uint32_t eax, ebx, ecx, edx;
        } *ol_gen_registers_t;
        
        typedef struct ol_cpu_model
        {
                uint32_t vendor_id, family;
                const uint8_t *model_name;
        }*ol_cpu_model_t;
        
        typedef struct ol_cpu
        {
                /* model info*/
                ol_cpu_model_t cpu_models[4];
                const uint8_t * vendor;
                                
                /* CPU device info */
                uint32_t flags; /* CPU flags */
                
                void (*ol_cpu_init)(struct ol_cpu *);
                
        } *ol_cpu_t;
        
        int
        ol_cpuid_available(ol_cpu_t cpu);

        static uint32_t
        ol_get_eflags(void);

        ol_cpu_t 
        ol_cpuid(void);
        
        extern uint32_t
        geteflags(void);


#ifdef	__cplusplus
}
#endif

#endif	/* CPU_H */

