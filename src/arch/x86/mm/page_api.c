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
 * \addtogroup x86_paging
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

        if (vpd[pde].present == 0 || vpt[pde] == NULL)
                return NULL;
        struct page_table* pt = vpt[pde];
        if (pt[pte].present == 0)
                return NULL;

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

int x86_pte_load_range(struct sys_mmu_range* range)
{
        if (range == NULL)
                return -E_NULL_PTR;

        /* If the range size == 0, there is nothing to be done */
        if (range->size == 0)
                return -E_SUCCESS;

        struct x86_pte_meta* meta = range->arch_data;
        if (meta == NULL)
        {
                /*
                 *  If the meta data structure doesn't exist, allocate a new one
                 *  and assume everything can be zero'd out.
                 */
#ifdef SLAB
                meta = mm_cache_alloc(x86_pte_meta_cache, 0);
#else
                meta = kmalloc(sizeof(*meta));
#endif
                if (meta == NULL)
                        panic("Unable to allocate paging meta data for segment");

                memset(meta, 0, sizeof(*meta));
                /* And update the arch data in the range structure */
                range->arch_data = meta;
        }

        /* Get an idea of the range we have to cover */
        addr_t from = (addr_t)range->virt;
        addr_t to = (addr_t)range->virt + range->size;

        /* Set up an index into the directory */
        idx_t idx = from >> 22;
        /* How many entries are to be set */
        idx_t limit = (range->size) >> 22;
        /* And the loop counter */
        idx_t cnt = 0;

        /* Maybe better to write this out somewhere else */
        addr_t four_meg = (1 << 22);
        /* Page table index */
        idx_t i = (from & (four_meg - 1)) >> 12;
        /* Page table index limit */
        idx_t last_entry = 0x3FF;

        if ((from & (four_meg - 1)) != 0 || (from >> 22) == (to >> 22))
        {
                /*
                 * If this range does not extend into another page table
                 * make sure we don't copy over too many table entries.
                 */
                if (cnt == limit)
                        last_entry = (to & (four_meg - 1) >> 12);

                /*
                 * If the first page table is not used completely, try to map it
                 * into the existing one
                 */
                if (!vpd[idx].present)
                {
                        /*
                         * If in the current setup this page table is not yet
                         * present, map in our own one.
                         */
                        vpd[idx] = meta->pd[idx];
                        vpt[idx] = meta->vpt[idx];
                        struct page_table* pt = vpt[idx];

                        for (; i <= last_entry; i++)
                        {
                                if (pt[i].unloaded)
                                {
                                        pt[i].unloaded = 0;
                                        pt[i].present = 1;
                                }
                        }
                }
                else
                {
                        /*
                         * If this page table is not actually used, just forget
                         * about it.
                         */
                        if (meta->pd[idx].present == 0)
                                goto skip1;

                        /* Set up the page table pointers */
                        struct page_table *pt = vpt[idx];
                        struct page_table* mpt = meta->vpt[idx];

                        /* And actually map the entries */
                        for (; i <= last_entry; i++)
                        {
                                if (pt[i].present)
                                        goto mapped;

                                pt[i] = mpt[i];
                                if (pt[i].unloaded)
                                {
                                        pt[i].present = 1;
                                        pt[i].unloaded = 0;
                                }
                        }

                        /*
                         * Since we copied over all our data, this page table
                         * has become redundant. That means the memory can be
                         * cleaned up now.
                         */
#ifdef SLAB
                        mm_cache_free(x86_pte_pt_cache, &(meta->vpt[idx]));
#else
                        kfree(&(meta->vpt[idx]));
#endif

                        /* Make sure the page directory present bit is set */
                        vpd[idx].present = 1;
                }
                /* Make sure the loop does not overwrite what we just did */
                idx++;
                cnt++;
        }
skip1:
        /*
         * Loop through the page directory entries and make them point to our
         * tables.
         *
         * This assumes we rounded our limit down, so it does should not include
         * the last page table, which could potentially only be partially used.
         */
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
        if ((to & (four_meg - 1)) != 0 && (from >> 22) != (to >> 22))
        {
                /* Set up loop parameters */
                i = 0;
                /* Make sure we don't map too many entries */
                last_entry = ((to & (four_meg - 1)) >> 12);
                if (last_entry == 0)
                        last_entry = 0x400;
                /* Set up page table pointers */
                struct page_table* pt = vpt[idx];
                struct page_table* mpt = meta->vpt[idx];

                /*
                 * If this page directory entry is the last one and partially
                 * mapped, but not also the first one, map this partially.
                 */
                if (!vpd[idx].present)
                {
                        /*
                         * Again, if this directory entry is not actively used
                         * in the current setup, just copy over the entry.
                         */
                        vpd[idx] = meta->pd[idx];
                        vpt[idx] = meta->vpt[idx];

                        for (; i <= last_entry; i++)
                        {
                                if (pt[i].unloaded)
                                {
                                        pt[i].unloaded = 0;
                                        pt[i].present = 1;
                                }
                        }
                        goto skip2;
                }
                /* If this entry is not used, just skip it .. */
                if (meta->pd[idx].present == 0)
                        goto skip2;

                /* And do the mapping */
                for (; i <= last_entry; i++)
                {
                        if (pt[i].present)
                        {
                                goto mapped;
                        }
                        pt[i] = mpt[i];
                        if (pt[i].unloaded)
                        {
                                pt[i].present = 1;
                                pt[i].unloaded = 0;
                        }
                }
                /*
                 * And since we copied everything over, this table has
                 * become redundant.
                 */
#ifdef SLAB
                mm_cache_free(x86_pte_pt_cache, &(meta->vpt[idx]));
#else
                kfree(&(meta->vpt));
#endif
        }

skip2:
        return -E_SUCCESS;
mapped:
        printf("PDE: %X\tPTE: %X\n", (int)idx, (int)i);
        printf("Problem area: %X\n", ((addr_t)idx << 22) | ((addr_t)i << 12));
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

        /* Make sure the task based page table data has been allocated */
        struct x86_pte_meta* meta = range->arch_data;
        if (meta == NULL)
        {
#ifdef SLAB
                meta = mm_cache_alloc(x86_pte_meta_cache, 0);
#else
                meta = kmalloc(sizeof(*meta));
#endif
                if (meta == NULL)
                        panic("Null pointers. Null pointers everywhere!");
                range->arch_data = meta;
                memset(meta, 0, sizeof(*meta));
        }

        /* Set up loop parameters and stuff */
        addr_t from = (addr_t)range->virt;
        addr_t to = (addr_t)range->virt + range->size;

        /* Idx = index into page directory */
        register idx_t idx = from >> 22;
        /* limit = number of pd entries to copy */
        idx_t limit = (to >> 22) - idx;
        /* cnt = how many pd entries have been copied */
        idx_t cnt = 0;

        addr_t four_meg = (1 << 22);
        /* i = index into page table */
        register idx_t i = (from & (four_meg - 1)) >> 12;
        /* last_entry indicates the last pt entry to copy */
        idx_t last_entry = 0x400;

        /*
         * For anybody wondering why task switches are expensive ...
         * Here is why!
         */
        if ((from & (four_meg - 1)) != 0 || (from >> 22) == (to >> 22))
        {
                /*
                 *  If the first page table is only partly used, make sure the
                 * existing entries don't get fubar'ed.
                 */
                if (vpd[idx].present == 0)
                {
                        /*
                         * If there are no existing entries, there is nothing to
                         * be fubar'ed. Just copy all of it asap.
                         */
                        meta->pd[idx].present = 0;
                        goto skip1;
                }
                /*
                 * If this is also the last page table, make sure we don't copy
                 * too many entries.
                 */
                if (cnt == limit)
                        last_entry = (to & (four_meg - 1) >> 12);

                /* Copy over the page directory information */
                meta->pd[idx] = vpd[idx];
                meta->vpt[idx] = vpt[idx];
                /* Set up loop parameters */
                struct page_table* pt = vpt[idx];

                for (; i <= last_entry; i++)
                {
                        if (pt[i].present)
                        {
                                pt[i].unloaded = 1;
                        }
                        pt[i].present = 0;
                }

                /*
                 * Make sure this entry will be ignored when unsetting the other
                 * tables.
                 */
                idx++;
                cnt++;

                /* If the page table was empty, besides our work, disable it */
                idx_t j = 0;
                for (; j < 0x400; j++)
                {
                        if (pt[j].present == 1)
                                goto skip1;
                }
                vpd[idx-1].present = 0;
        }
skip1:
        for (; cnt < limit; cnt++, idx++)
        {
                /*
                 * Disable the page tables we know are filled completely.
                 */
                meta->pd[idx] = vpd[idx];
                meta->vpt[idx] = vpt[idx];

                /*
                 * Disable the page directory entry here, by simply writing 0
                 * to the entry. It is quicker than flipping only one bit, as
                 * that would require the data to also be read from memory.
                 */
                *(int*)&vpd[idx] = 0;
        }
        /*
         * If the last page table wasn't used completely, make sure we take the
         * table entries out as well and leave the rest
         */
        i = 0;
        if ((to & (four_meg - 1)) != 0 && (from >> 22) != (to >> 22))
        {
                /* If this entry isn't actually in use, just skip over it */
                if (vpd[idx].present == 0)
                {
                        meta->pd[idx].present = 0;
                        goto skip2;
                }
                /* Copy page directory data */
                meta->pd[idx] = vpd[idx];
                meta->vpt[idx] = vpt[idx];

                /* Setup page table pointer */
                struct page_table* pt = vpt[idx];

                /*
                 * Make sure we get all the page table entries, but don't go too
                 * far.
                 */
                last_entry = ((to & (four_meg - 1)) >> 12);
                for (; i <= last_entry; i++)
                {
                        if (pt[i].present)
                        {
                                pt[i].unloaded = 1;
                                pt[i].present = 0;
                        }
                }
                /*
                 * If this range is otherwise not in use, disable it in the page
                 * directory.
                 */
                for (i = last_entry; i <= 0x400; i++)
                {
                        if (pt[i].present != 0)
                                goto skip2;
                }
                *(int*)&vpd[idx] = 0;
        }

skip2:
        /* Make sure we invalidate the tlb, we don't want to be using old data*/
        asm volatile ("mov %%cr3, %%eax\n\t"
                      "mov %%eax, %%cr3\n\t"
                      ::: "%eax", "memory" );

        return -E_SUCCESS;
}

int
x86_page_cleanup_range(struct sys_mmu_range* range)
{
        if (range == NULL)
                return -E_NULL_PTR;

        struct x86_pte_meta* meta = range->arch_data;
        if (meta == NULL)
                return -E_SUCCESS;

        /* Set up loop parameters and stuff */
        addr_t from = (addr_t)range->virt;
        addr_t to = (addr_t)range->virt + range->size;

        /* Idx = index into page directory */
        register idx_t idx = from >> 22;
        /* limit = number of pd entries to copy */
        idx_t limit = (to >> 22) - idx;
        /* cnt = how many pd entries have been copied */
        idx_t cnt = 0;

        addr_t four_meg = (1 << 22);
        /* i = index into page table */
        register idx_t i = (from & (four_meg - 1)) >> 12;
        /* last_entry indicates the last pt entry to copy */
        idx_t last_entry = 0x400;

        for (; cnt <= limit; cnt++, idx++)
        {
                if (cnt == limit)
                        last_entry = (to & (four_meg - 1) >> 12);
                for (; i < last_entry && meta->pd[idx].present; i++)
                {
                        struct page_table* pt = meta->vpt[idx];
                        if (!pt[i].present && !pt[i].unloaded)
                                continue;

                        addr_t phys = pt[i].pageIdx;
                        phys <<= 12;

                        page_free((void*)phys);
                }
                i = 0;

                if (meta->pd[idx].present)
                {
                        // Unload page table ...
                        meta->pd[idx].present = 0;
#ifdef SLAB
                        mm_cache_free(x86_pte_pt_cache, meta->vpt[idx]);
#else
                        kfree(meta->vpt[idx]);
#endif
                }
        }


#ifdef SLAB
        mm_cache_free(x86_pte_meta_cache, meta);
#else
        kfree(meta);
#endif
        range->arch_data = NULL;

        return -E_SUCCESS;
}

/**
 * @}
 * \file
 */
