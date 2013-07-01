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
#include <andromeda/system.h>
#include <arch/x86/pte.h>
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

struct page_dir* pd = NULL; // Physical address of page directory
struct page_dir *vpd; // Virtual address of page directory
void* vpt[1024];  // Virtual addresses of page tables

#ifdef SLAB
struct mm_cache* x86_pte_pt_cache = NULL;
struct mm_cache* x86_pte_meta_cache = NULL;
#endif

extern struct page_table page_table_boot;
extern struct page_dir page_dir_boot;

int x86_pte_init()
{
        vpd = &page_dir_boot + THREE_GIB;
        pd = &page_dir_boot;
        addr_t virt_pt = (addr_t)&page_table_boot;
        virt_pt += THREE_GIB;
        memset(vpt, 0, sizeof(vpt));
#ifdef PT_DBG
        printf("vpt addr: %X\n", vpt);
#endif
        int i = 768;
        unsigned int inc = 1024 * sizeof(page_table_boot);
        for (; i < 1024; i++, virt_pt += inc)
        {
                vpt[i] = (void*)virt_pt;
        }
        memset(&pte_cnt, 0, sizeof(pte_cnt));
#ifdef SLAB
        x86_pte_pt_cache = mm_cache_init("PT_cache",
                        1024*sizeof(struct page_table),
                        1024*sizeof(struct page_table),
                        NULL,
                        NULL);
        x86_pte_meta_cache = mm_cache_init("PTE_meta_cache",
                        sizeof(struct x86_pte_meta),
                        sizeof(struct x86_pte_meta),
                        NULL,
                        NULL);
        if (x86_pte_pt_cache == NULL || x86_pte_meta_cache == NULL)
                panic("Unable to initialise the pte memory caches!");
#endif
        return -E_SUCCESS;
}

void* x86_pte_get_phys(void* virt)
{
        addr_t v = (addr_t)virt >> 12;
        int pte = v & 0x3FF;
        int pde = (v >> 10) & 0x3FF;

        struct page_table* pt = vpt[pde];
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

        x86_pte_load_range(s->pages);

        return -E_SUCCESS;
}

int x86_pte_unset_segment(struct vm_segment* s)
{
        if (s == NULL || s->pages == NULL)
                return -E_NULL_PTR;

        return x86_pte_unload_range(s->pages);
}

/** \todo Write some bloody good comments for this code */
int x86_pte_load_range(struct sys_mmu_range* range)
{
        if (range == NULL)
                return -E_NULL_PTR;

        if (range->arch_data == NULL)
        {
#ifdef SLAB
                range->arch_data = mm_cache_alloc(x86_pte_meta_cache,0);
#else
                range->arch_data = kmalloc(sizeof(struct x86_pte_meta));
#endif
                if (range->arch_data == NULL)
                        panic("Unable to allocate paging meta data for segment");

                memset(range->arch_data, 0, sizeof(struct x86_pte_meta));
        }

        struct x86_pte_meta* meta = range->arch_data;
        if (range->size == 0)
                return -E_SUCCESS;

        addr_t from = (addr_t)range->virt;
        addr_t to = (addr_t)range->virt + range->size;

        idx_t idx = from >> 22;
        idx_t limit = (to-from) >> 22;
        idx_t cnt = 0;

        addr_t four_meg = (1 << 22);
        idx_t i = (from & (four_meg - 1)) >> 12;
        idx_t last_entry = 0x400;

        if ((from & (four_meg - 1)) != 0)
        {
                if (!vpd[idx].present)
                {
                        vpd[idx] = meta->pd[idx];
                        vpt[idx] = meta->vpt[idx];
                }
                else
                {
                        if (meta->pd[idx].present == 0)
                                goto skip1;
                        struct page_table *pt = vpt[idx];
                        struct page_table* mpt = meta->vpt[idx];
                        if (cnt == limit)
                                last_entry = (to & (four_meg - 1) >> 12);

                        for (; i < last_entry; i++)
                        {
                                if (pt[i].present)
                                        goto mapped;
                                pt[i] = mpt[i];
                        }

#ifdef SLAB
                        mm_cache_free(x86_pte_pt_cache, &(meta->vpt[idx]));
#else
                        kfree(&(meta->vpt[idx]));
#endif

                        vpd[idx].present = 1;
                }
                idx++;
                cnt++;
        }
skip1:
        for (; cnt < limit; cnt++, idx++)
        {
                if (vpd[idx].present)
                        goto mapped;
                if (!meta->pd[idx].present)
                        continue;

                vpd[idx] = meta->pd[idx];
                vpt[idx] = meta->vpt[idx];

                vpd[idx].present = 1;
        }
        i = 0;
        if ((to & (four_meg - 1)) != 0 && (from >> 22) != (to >> 22))
        {
                if (!vpd[idx].present)
                {
                        vpd[idx] = meta->pd[idx];
                        vpt[idx] = meta->vpt[idx];
                }
                else
                {
                        if (meta->pd[idx].present == 0)
                                goto skip2;
                        last_entry = (to & (four_meg - 1) >> 12);
                        struct page_table* pt = vpt[idx];
                        struct page_table* mpt = meta->vpt[idx];
                        for (; i < last_entry; i++)
                        {
                                if (pt[i].present)
                                        goto mapped;
                                pt[i] = mpt[i];
                        }
#ifdef SLAB
                        mm_cache_free(x86_pte_pt_cache, &(meta->vpt[idx]));
#else
                        kfree(&(meta->vpt));
#endif
                }
        }

skip2:
        return -E_SUCCESS;
mapped:
        panic("Attempting to map already mapped area");
        return -E_GENERIC;
}

/** \todo And this code can use some comments as well! */
int x86_pte_unload_range(struct sys_mmu_range* range)
{
        if (range == NULL)
                return -E_NULL_PTR;

        if (range->size == 0)
                return -E_SUCCESS;

        if (range->arch_data == NULL)
        {
#ifdef SLAB
                range->arch_data = mm_cache_alloc(x86_pte_meta_cache, 0);
#else
                range->arch_data = kmalloc(sizeof(struct x86_pte_meta));
#endif
                if (range->arch_data == NULL)
                        panic("Null pointers. Null pointers everywhere!");
        }

        struct x86_pte_meta* meta = range->arch_data;

        addr_t from = (addr_t)range->virt;
        addr_t to = (addr_t)range->virt + range->size;

        idx_t idx = from >> 22;
        idx_t limit = (to >> 22) - idx;
        idx_t cnt = 0;

        addr_t four_meg = (1 << 22);
        idx_t i = (from & (four_meg - 1)) >> 12;
        idx_t last_entry = 0x400;

        /*
         * For anybody wondering why task switches are expensive ...
         * Here is why!
         */
        if ((from & (four_meg - 1)) != 0)
        {
                if (vpd[idx].present == 0)
                {
                        meta->pd[idx].present = 0;
                        goto skip1;
                }
                if (idx == limit)
                        last_entry = (to & (four_meg - 1) >> 12);

                meta->pd[idx] = vpd[idx];
                meta->vpt[idx] = vpt[idx];
                struct page_table* pt = vpt[idx];

                for (; i < last_entry; i++)
                {
                        pt[i].present = 0;
                }

                idx_t j = 0;
                for (; j < i; j++)
                {
                        if (pt[j].present == 1)
                                goto skip1;
                }
                vpd[idx].present = 0;
                idx++;
                cnt++;
        }
skip1:
        for (; cnt < limit; cnt++, idx++)
        {
                if (vpd[idx].present == 0)
                {
                        meta->pd[idx].present = 0;
                        continue;
                }
                meta->pd[idx] = vpd[idx];
                meta->vpt[idx] = vpt[idx];

                /*
                 * Leave page table entry bits enabled here. The present bit
                 * will be turned off in the page directory and it makes for
                 * quicker loading and unloading.
                 */

                vpd[idx].present = 0;
        }
        i = 0;
        if ((to & (four_meg - 1)) != 0 && (from >> 22) != (to >> 22))
        {
                if (vpd[idx].present == 0)
                {
                        meta->pd[idx].present = 0;
                        goto skip2;
                }
                meta->pd[idx] = vpd[idx];
                meta->vpt[idx] = vpt[idx];

                struct page_table* pt = vpt[idx];

                last_entry = (to & (four_meg - 1) >> 12);
                for (; i < last_entry; i++)
                {
                        pt[i].present = 0;
                }
                for (i = last_entry; i < 0x400; i++)
                {
                        if (pt[i].present != 0)
                                goto skip2;
                }
                vpd[idx].present = 0;
        }

skip2:
        asm volatile ("mov %%cr3, %%eax\n\t"
             "mov %%eax, %%cr3\n\t"
              ::: "%eax" );

        return -E_SUCCESS;
}

/**
 * @} \file
 */
