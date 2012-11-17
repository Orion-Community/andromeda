/*
 * Andromeda
 * Copyright (C) 2012  Bart Kuivenhoven
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

#include <defines.h>
#include <mm/pte.h>
#include <mm/page_alloc.h>

/**
 * \AddToGroup PTE
 * @{
 */

/**
 * \fn pte_alloc
 * \brief Allocate a new pte descriptor for a specific task
 * \param pid
 * \brief The task to connect to
 * \return The pte descriptor created or NULL
 * \warning This can only be used to create userland tasks, kernel is static
 */

struct pte_descriptor*
pte_alloc(int pid)
{
        struct pte_descriptor* p = kalloc(sizeof(*p));
        if (p == NULL)
                return NULL;

        memset(p, 0, sizeof(*p));
        p->pid = pid;
        p->cpl = PTE_CPL_USER;
        return p;
}

/**
 * \fn pte_new_segment
 * \brief Add a new segment to a descriptor
 * \param virt
 * \brief The virtual pointer to map to
 * \param p
 * \brief The descriptor to put the segment into
 * \return the segment which is added to the descriptor
 */
struct pte_segment*
pte_new_segment(void* virt, struct pte_descriptor* p)
{
        struct pte_segment* s = kalloc(sizeof(*s));
        if (s == NULL)
                return NULL;

        memset(s, 0, sizeof(*s));
        s->virt_base = virt;
        s->next = p->segments;
        p->segments = s;
        /**
         * \todo Add physical page allocation here
         */

        return s;
}

/**
 * \fn pte_free_page
 * \brief Free the physical page, if present
 * \param s
 * \brief The segment to work with
 * \return A standard error code
 */
int pte_free_page(struct pte_segment* s)
{
        if (s->pte == NULL)
                return -E_SUCCESS;

        /**
         * \todo Iterate through page tables to find allocated pages
         * \todo Free up those allocated pages
         */

        return -E_NOFUNCTION;
}

/**
 * \fn pte_segment_map
 * \brief Map a physical location into a segment
 * \param s
 * \brief The segment to map into
 * \param phys
 * \brief The pages to map
 * \return A standard error code
 */
int
pte_segment_map(struct pte_segment* s, void* phys)
{
        /**
         * \todo Allocate yourself a page table, if not already present
         * \todo Point the segment to the table.
         */
        return -E_NOFUNCTION;
}

/**
 * \fn pte_core_map
 * \brief Map a physical location into core memory
 * \param s
 * \brief The segment to work with
 * \param phys
 * \brief The pages to map
 * \return A standard error code
 */
int
pte_core_map(struct pte_segment* s, void* phys)
{
        /**
         * \todo Find yourself the page table to point to
         * \todo point the segment to the table
         * \todo Kernel, so always present, so insert page table
         */
        return -E_NOFUNCTION;
}

/**
 * \fn pte_free
 * \brief Clean up a pte descriptor and free it
 * \param p
 * \brief the descriptor to clean up
 * \return A standard error code
 * \warning If the error code isn't -E_SUCCESS, the task still exists
 */
int
pte_free(struct pte_descriptor* p)
{
        struct pte_segment* this = p->segments;
        struct pte_segment* next = this->next;

        for (; this != NULL; this = next, next = next->next)
        {
                if (pte_free_page(this) != -E_SUCCESS)
                {
                        /*
                         * We have just created a ghost task.
                         * For some reason, or another, this task we can't get
                         * rid of.
                         */
                        p->segments = this;
                        return -E_GENERIC;
                }
                kfree(this);
        }
        kfree(p);
        return -E_SUCCESS;
}

/**
 * @}
 * \file
 */
