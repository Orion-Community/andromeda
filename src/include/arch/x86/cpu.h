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

#define OL_CPUID_AVAILABLE 0x1
#define OL_CPUID_TEST_BIT 0x200000

/* MultiProcessor defines */
#define OL_CPU_MP_FPS_SIGNATURE 0x5F504D5F
#define OL_CPU_MP_CONFIG_TABLE_HEADER_SIGNATURE 0x50434d50

#ifndef CPU_H
#define	CPU_H

#ifdef	__cplusplus
extern "C"
{
#endif

        typedef uint8_t ol_lock_t;
    
        struct ol_gen_regs
        {
                uint32_t eax, ebx, ecx, edx;
        } __attribute__((packed));
        typedef struct ol_gen_regs *ol_gen_registers_t;
        
        typedef struct ol_cpu_model
        {
                uint32_t vendor_id, family;
                const uint8_t *model_name;
        }*ol_cpu_model_t;
        
        
        typedef struct ol_cpu
        {
                /* model info*/
                ol_cpu_model_t cpu_models[4];
                const uint8_t *vendor;
                                
                /* CPU device info */
                uint8_t flags;

                void (*lock)(ol_lock_t*);
                void (*unlock)(ol_lock_t*);
                
        } *ol_cpu_t;
        
        struct ol_mp_config_table_header
        {
                
        } __attribute__((packed));
        typedef struct ol_mp_config_table_header *ol_mp_config_table_header_t;
        
        struct ol_cpu_mp_fps
        {
                uint32_t signature;
                ol_mp_config_table_header_t conf_table;
                uint8_t len, spec_rev, checksum, mp_feat1;
                uint8_t mp_feat2, mp_feat3, mp_feat4, mp_feat5;
        } __attribute__((packed));
        typedef struct ol_cpu_mp_fps *ol_cpu_mp_fps_t;

/* CPU feature functions */        
        static int
        ol_cpuid_available(ol_cpu_t);

        void
        ol_cpu_init(ol_cpu_t);
        
        ol_gen_registers_t 
        ol_cpuid(uint32_t func);
        
        uint32_t
        ol_get_eflags(void);
        
        void
        ol_set_eflags(uint32_t);

        /* LOCKS */
        void /* lock spin lock */
        ol_mutex_lock(ol_lock_t*);
        
        void /* spin release */
        ol_mutex_release(ol_lock_t*);
        
        /* CPUID */
        static ol_gen_registers_t
        __ol_cpuid(volatile ol_gen_registers_t);
        
        static void *
        ol_cpu_mp_search_config_table(char* , int);
        
        int
        ol_get_mp_config_header();

#ifdef	__cplusplus
}
#endif

#endif	/* CPU_H */

