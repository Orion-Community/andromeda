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
#include <mm/vm.h>
#include <mm/page_alloc.h>

/**
 * \AddToGroup VM
 * @{
 */

/**
 * \fn vm_alloc
 * \brief Allocate a new vm descriptor for a specific task
 * \param pid
 * \brief The task to connect to
 * \return The pte descriptor created or NULL
 * \warning This can only be used to create userland tasks, kernel is static
 */

struct vm_descriptor*
vm_alloc(int pid)
{
        struct vm_descriptor* p = kalloc(sizeof(*p));
        if (p == NULL)
                return NULL;

        memset(p, 0, sizeof(*p));
        p->pid = pid;
        p->cpl = VM_CPL_USER;
        return p;
}

/**
 * \fn vm_new_segment
 * \brief Add a new segment to a descriptor
 * \param virt
 * \brief The virtual pointer to map to
 * \param p
 * \brief The descriptor to put the segment into
 * \return the segment which is added to the descriptor
 */
struct vm_segment*
vm_new_segment(void* virt, struct vm_descriptor* p)
{
        struct vm_segment* s = kalloc(sizeof(*s));
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
 * \fn vm_free_page
 * \brief Free the physical page, if present
 * \param s
 * \brief The segment to work with
 * \return A standard error code
 */
int vm_segment_free(struct vm_segment* s)
{
        /**
         * \todo Iterate through page tables to find allocated pages
         * \todo Free up those allocated pages
         */

        return -E_NOFUNCTION;
}

/**
 * \fn vm_segment_map
 * \brief Map a physical location into a segment
 * \param s
 * \brief The segment to map into
 * \param phys
 * \brief The pages to map
 * \return A standard error code
 */
int
vm_segment_map(struct vm_segment* s, struct mm_page_descriptor* p)
{
        /**
         * \TODO: If pte_descriptor == loaded, map segment into page directory
         */
        return -E_NOFUNCTION;
}

/**
 * \fn vm_free
 * \brief Clean up a vm descriptor and free it
 * \param p
 * \brief the descriptor to clean up
 * \return A standard error code
 * \warning If the error code isn't -E_SUCCESS, the task still exists
 */
int
vm_free(struct vm_descriptor* p)
{
        struct vm_segment* this = p->segments;
        struct vm_segment* next = this->next;

        for (; this != NULL; this = next, next = next->next)
        {
                if (vm_segment_free(this) != -E_SUCCESS)
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
 * \fn vm_get_phys
 * \brief Find the physical address for a virtual one
 * \param vm
 * \brief The descriptor to look in
 * \param virt
 * \brief The virtual address to find
 * \return the physical page pointer
 * \todo Implement the arch api call to look inside the tables
 */
void*
vm_get_phys(struct vm_descriptor* vm, void* virt)
{
        if (vm == NULL || virt == NULL || !PAGE_ALIGNED((int)vm))
                return NULL;

#if 0
        addr_t v = (addr_t)virt;

        struct vm_segment* carriage = vm->segments;
        for (; carriage != NULL; carriage = carriage->next)
        {
                addr_t vm_base = (addr_t)carriage->virt_base;
                addr_t vm_end = (addr_t)carriage->virt_base + carriage->size;
                if ((addr_t)virt >= vm_base && (addr_t)virt < vm_end)
                {
                        /* Call the architecture api to get the address */
                        addr_t tmp = (addr_t)virt - vm_base;
                        tmp /= PAGESIZE;
                        return (void*)pte_get_phys(virt, carriage);
                }
        }
#endif
        return pte_get_phys(virt);
        return NULL;
}

int
vm_load_task()
{
        return -E_NOFUNCTION;
}

int
vm_unload_task()
{
        return -E_NOFUNCTION;
}

/**
 * @}
 * \file
 */
