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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
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
#include <arch/x86/pte.h>
#include "page_table.h"

#ifdef SLOB
#include <mm/heap.h>
#endif

/**
 * \addtogroup x86_paging
 * @{
 */

static volatile mutex_t pte_lock = mutex_unlocked;

/**
 * \fn x86_cnt_pt_entries
 * \param pte
 * \return
 */
static int
x86_cnt_pt_entries(struct page_table* pte)
{
        mutex_lock(&pte_lock);
        int cnt, i;
        for (cnt = 0, i = 0; i < 0x400; i++)
                cnt += pte[i].present;

        mutex_unlock(&pte_lock);

        return cnt;
}

/**
 * \fn x86_pte_set_page
 * \brief Set the page table entry to the correct value
 * \param virt
 * \param phys
 * \param cpl
 * \param pte
 */
static int x86_pte_set(void* phys, int cpl, struct page_table* pte)
{
        /* Make sure the pointer isn't null and is dword aligned */
        if (pte == NULL || ((int)pte & 0x3) != 0)
                panic("Incorrect conditions for setting pte");

        pte->userMode = (cpl == 0) ? 0 : 1;
        pte->pageIdx = (int)phys >> 12;
        pte->present = 1;
        pte->rw = 1;

        return -E_SUCCESS;
}

/**
 * \fn x86_pte_unset_page
 * \brief Set the page table entry as being cleared
 */
static int x86_pte_unset(struct page_table* pte)
{
        if (pte == NULL || ((int)pte & 0x3) != 0)
                panic("Incorrect conditions for unsetting pte");

        pte->present = 0;
        return -E_SUCCESS;
}

/**
 * \fn x86_pte_set_pt
 * \brief Set a page table reference into the page dir.
 * \param pt
 * \brief The physical page pointer to the page table.
 * \param idx
 * \brief The index to put the entry.
 */
static int x86_pte_set_pt(struct page_table* pt, int idx)
{
        if (idx >= 1024 || pt == NULL)
                return -E_INVALID_ARG;

        vpd[idx].pageIdx = (int)pt >> 12;
        vpd[idx].present = 1;
        vpd[idx].userMode = 1;
        vpd[idx].rw = 1;
        return -E_SUCCESS;
}

/**
 * \fn x86_pte_unset_pt
 * \brief Disable a page table
 * \param idx
 * \brief The page table to disable
 */
static int x86_pte_unset_pt(int idx)
{
        if (idx >= 1024)
                return -E_INVALID_ARG;

        /* find the entry and mark in not present */
        vpd[idx].present = 0;
        return -E_SUCCESS;
}

/**
 * \fn x86_pte_set_page
 * \brief Map a virtual address to a physical one
 * \param virt
 * \param phys
 * \param cpl
 * \return A standard error code
 */
int x86_pte_set_page(void* virt, void* phys, int cpl)
{
        if (virt == NULL || phys == NULL ||(((int)virt|(int)phys) & 0xFFF) != 0)
                return -E_INVALID_ARG;

        addr_t v = (addr_t)virt >> 12;

        int pte = v & 0x3FF;
        int pde = (v >> 10) & 0x3FF;

        struct page_table* pt;
        mutex_lock(&pte_lock);
        pt = vpt[pde];
        if (pt == NULL || !vpd[pde].present)
        {
#ifdef SLAB
                pt = mm_cache_alloc(x86_pte_pt_cache, 0);
#elif defined SLOB
                pt = alloc(sizeof(*pt)*1024, TRUE);
#endif
                if (pt == NULL)
                        panic("Out of memory! (And unicorns)");
                memset(pt, 0, sizeof(*pt)*1024);
                x86_pte_set_pt(get_phys(0,pt), pde);
                vpt[pde] = pt;
        }

        x86_pte_set(phys, cpl, &pt[pte]);

        asm("invlpg (%0)" : : "r" (virt) : "memory");

        mutex_unlock(&pte_lock);

        return -E_SUCCESS;
}

/**
 * \fn x86_pte_unset_page
 * \brief Disable access to this one virtual address
 * \param virt
 * \return A standard error code
 */
int x86_pte_unset_page(void* virt)
{
        if (virt == NULL && ((addr_t)virt & 0xFFF) != 0)
                panic("Invalid pointer in x86_pte_unset_page");
        addr_t v = (addr_t)virt >> 12;

        int pte = v & 0x3FF;
        int pde = (v >> 10) & 0x3FF;

        mutex_lock(&pte_lock);
        struct page_table* pt;
        if ((pt = vpt[pde]) == NULL)
        {
                mutex_unlock(&pte_lock);
                return -E_SUCCESS;
        }

        int ret = x86_pte_unset(&pt[pte]);
        asm ("invlpg (%0)" :: "r" (virt) : "memory");
        if (x86_cnt_pt_entries(pt) <= 0)
                x86_pte_unset_pt(pde);

        mutex_unlock(&pte_lock);

        return ret;
}

int idx = 0;

void
x86_pagefault(isrVal_t* registers)
{
        addr_t fault_addr = 0;
        asm ("mov %%cr2, %%eax\n\t"
             "mov %%eax, %0\n\t"
             : "=r" (fault_addr)
             :
             : "%eax", "memory");

        /*
        addr_t pde = fault_addr >> 22;
        addr_t pte = (fault_addr >> 12) & 0x3FF;
        int* pt = vpt[pde];
        printf("PF #%i\n", ++idx);
        printf("Fault addr: %X\n", (int)fault_addr);
        printf("pde: %X - %X - %X\n", *(int*)&vpd[pde], (int)pde, pd);
        printf("pte: %X - %X - %X\n", pt[pte], (int)pte, x86_pte_get_phys(&pt));
        printf("Err code: %X\n", registers->errCode);
        */

        if (registers->errCode & 4)
        {
                /* User space page faults */
                if (registers->errCode & 2)
                        vm_user_fault_write(fault_addr,(registers->errCode & 1));
                else
                        vm_user_fault_read(fault_addr, (registers->errCode & 1));
        }
        else
        {
                /* Kernel space page faults */
                if (registers->errCode & 2) {
                        vm_kernel_fault_write(fault_addr,(registers->errCode&1));
                } else {
                        vm_kernel_fault_read(fault_addr,(registers->errCode&1), registers->eip);
                }
        }
//      printf("End of interrupt: #%i\n", idx--);
}

/**
 * @}
 * \file
 */
