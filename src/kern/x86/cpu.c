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
#include <arch/x86/cpu.h>

ol_lock_t lock;

int
ol_cpuid_available(ol_cpu_t cpu)
{
        register uint32_t flags;
        register uint32_t flags2;
        
        cpu->lock(lock);
        flags = ol_get_eflags();
        ol_set_eflags(flags^OL_CPUID_TEST_BIT);
        flags2 = ol_get_eflags();
        ol_set_eflags(flags); /* restore flags */
        
        if((flags>>21)&1)
        {
                cpu->unlock(lock);
                return 0;
        }
        else
        {
                cpu->unlock(lock);
                return -1;
        }
}

ol_cpu_t 
ol_cpuid(void)
{
        
}

void
ol_cpu_init(ol_cpu_t cpu)
{
        cpu->flags = 0;
        cpu->lock = &ol_mutex_lock;
        cpu->unlock = &ol_mutex_release;
        cpu->flags |= ol_cpuid_available(cpu);
}
