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

extern ol_lock_t lock;

uint8_t
ol_cpuid_available(ol_cpu_t cpu)
{
        cpu->lock(lock);
        return cpu->geteflags();
        cpu->unlock(lock);
}

ol_cpu_t 
ol_cpuid(void)
{
        
}

void
ol_cpu_init(ol_cpu_t cpu)
{
        cpu->geteflags = &geteflags;
        cpu->flags = 0;
        cpu->lock = &mutex_lock;
        cpu->unlock &mutex_release;
        cpu->flags |= ol_cpuid_available(cpu);
}
