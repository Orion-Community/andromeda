/*
 * Andromeda
 * Copyright (C) 2013  Bart Kuivenhoven
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <mm/page_alloc.h>
#include <mm/vm.h>
#include <types.h>
#include <andromeda/error.h>

/**
 * \AddToGroup paging
 * @{
 */

struct page_dir* pd = NULL;
struct page_dir *spd;
void* vpd[1024];
extern struct page_table page_table_boot;
extern struct page_dir page_dir_boot;

int x86_pte_init()
{
        spd = &page_dir_boot + THREE_GIB;
        pd = &page_dir_boot;
        addr_t vpt = (addr_t)&page_table_boot;
        vpt += THREE_GIB;
        memset(vpd, 0, sizeof(vpd));
#ifdef PT_DBG
        printf("vpt addr: %X\n", vpt);
#endif
        int i = 768;
        unsigned int inc = 1024 * sizeof(page_table_boot);
        for (; i < 1024; i++, vpt += inc)
        {
                vpd[i] = (void*)vpt;
        }
        return -E_SUCCESS;
}

void* pte_get_phys(void* virt, struct vm_segment* s)
{
        return NULL;
}

int x86_pte_set(struct vm_segment* s)
{
        return -E_NOFUNCTION;
}

int x86_pte_unset(struct vm_segment* s)
{
        return -E_NOFUNCTION;
}

/**
 * @} \file
 */
