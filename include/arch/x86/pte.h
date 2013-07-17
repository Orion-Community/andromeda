/*
    Andromeda
    Copyright (C) 2013  Bart Kuivenhoven

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __ARCH_X86_PTE_H
#define __ARCH_X86_PTE_H

#include <andromeda/system.h>

int x86_pte_unset_page(void* virt);
int x86_pte_set_page(void* virt, void* phys, int cpl);
int x86_pte_unload_range(struct sys_mmu_range* range);
int x86_pte_load_range(struct sys_mmu_range* range);
void* x86_pte_get_phys(void* virt);
int x86_page_cleanup_range(struct sys_mmu_range* range);

#endif
