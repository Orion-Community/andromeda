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
#include <mm/paging.h>
#include <mm/vm.h>
#include <types.h>
#include <andromeda/error.h>
#include "page_table.h"

/**
 * \AddToGroup paging
 * @{
 *
 * \todo Write segment setting.
 * \todo Write segment disabling.
 * \todo Write page table allocation code.
 * This will probably just be a slab, mapped to physical addresses. Still it
 * needs to be done.
 * \todo Write a high level interface to call all this.
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
        memset(&pte_cnt, 0, sizeof(pte_cnt));
        return -E_SUCCESS;
}

void* x86_pte_get_phys(void* virt)
{
        addr_t v = (addr_t)virt >> 12;
        int pte = v & 0x3FF;
        int pde = (v >> 10) & 0x3FF;

        struct page_table* pt = vpd[pde];
        addr_t ret = pt[pte].pageIdx;
        ret <<= 12;

        ret += (addr_t)virt & 0xFFF;

        return (void*)ret;
}

int x86_pte_map(void* virt, void* phys, int cpl)
{
        return x86_pte_set_page(virt, phys, cpl);
}

int x86_pte_set_segment(struct vm_segment* s)
{
        if (s == NULL || s->pages == NULL)
                return -E_NULL_PTR;

        x86_pte_set_range(s->pages);

        return -E_SUCCESS;
}

int x86_pte_unset_segment(struct vm_segment* s)
{
        if (s == NULL || s->pages == NULL)
                return -E_NULL_PTR;

        x86_pte_reset_range(s->pages);

        return -E_SUCCESS;
}

/**
 * @} \file
 */
