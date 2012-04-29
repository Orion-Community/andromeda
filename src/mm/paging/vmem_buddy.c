/*
 * Andromeda
 * Copyright (C) 2011  Bart Kuivenhoven
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
/**
 * \AddToGroup vmem
 * @{
 */
#include <mm/vmem.h>
#include <mm/paging.h>
#include <andromeda/error.h>

int vmem_buddy_set(struct vmem_buddy* buddy);

/**
 * \fn vmem_buddy_system_init
 * \brief Create a new region allocatable by the buddy system
 * \param base_ptr
 * \brief The pointer to the first allocatable object
 * \warning This must be page aligned
 * \param size
 * \brief The size of the system
 * \warning The size must be a multiple of PAGESIZE
 * \warning The size must be at least 0x80 pages (0.5 MiB on intel)
 * \return A pointer to the newly created buddy system descriptor
 */
struct vmem_buddy_system*
vmem_buddy_system_init(void* base_ptr, size_t size)
{
        if ((addr_t)base_ptr % PAGESIZE != 0 || size % PAGESIZE != 0)
                goto err;

        if (log2i(size/PAGESIZE) < BUDDY_NO_POWERS)
        {
#ifdef X86
        if ((1>>(log2i(size<<12)) >> 12) % (addr_t)base_ptr != 0)
                goto err;
#else
        if (((int)pow(2, log2i(size/PAGESIZE))*PAGESIZE) % (addr_t)base_ptr != 0)
                goto err;
#endif
        }
        else
        {
                if ((addr_t)base_ptr % ((int)pow(2, BUDDY_NO_POWERS)*PAGESIZE) != 0)
                        goto err;
        }
        if ((addr_t)base_ptr % size != 0)
                goto err;

        if (size < 0x80*PAGESIZE)
                goto err;

        struct vmem_buddy_system* system = kalloc(sizeof(*system));
        if (system == NULL)
                goto err;

        memset(system, 0, sizeof(*system));

        size_t remaining = size;
        size_t allocated = 0;
        for (; PAGESIZE < remaining; remaining -= allocated)
        {
                if (log2i(remaining>>12) > BUDDY_NO_POWERS-1)
                        allocated = pow(2, BUDDY_NO_POWERS-1);
                else
                        allocated = pow(2, log2i(remaining>>12));
                allocated *= PAGESIZE;
                struct vmem_buddy* buddy = kalloc(sizeof(*buddy));
                if (buddy == NULL)
                        goto err_buddy;

                memset(buddy, 0, sizeof(*buddy));
                buddy->size = allocated;
                buddy->ptr = base_ptr;
                buddy->system = system;
                base_ptr += allocated;
                vmem_buddy_set(buddy);
                debug("Remaining: %X\tallocated: %X\n", remaining, allocated);
        }
        return system;

err_buddy:
{
        int i = 0;
        struct vmem_buddy* cariage = NULL;
        struct vmem_buddy* next = NULL;
        for (; i < BUDDY_NO_POWERS; i++)
        {
                cariage = system->buddies[i];

                for (; cariage != NULL; cariage = next)
                {
                        next = cariage->next;
                        kfree(cariage);
                }
        }
        memset(system, 0, sizeof(*system));
        kfree(system);
}
err:
        return NULL;
}

/**
 * \todo Build a function that removes a buddy from its list
 * \fn vmem_buddy_purge
 * \brief Purge the given buddy from the system it points to
 * \param buddy
 * \brief The buddy to purge
 * \return A generic error code
 */
int
vmem_buddy_purge(struct vmem_buddy* buddy)
{
        if (buddy == NULL)
                return -E_NULL_PTR;

        if (buddy->prev != NULL)
                buddy->prev->next = buddy->next;
        else
        {
                size_t size = buddy->size >> 12;
                idx_t buddy_power = log2i(size);
                buddy->system->buddies[buddy_power] = buddy->next;
        }
        if (buddy->next != NULL)
                buddy->next->prev = buddy->prev;

        return -E_SUCCESS;
}

/**
 * \fn vmem_buddy_set
 * \brief Insert a buddy into the list of the system pointed to by the buddy
 * \param buddy
 * \brief The buddy to inser
 * \return A generic error code
 */
int
vmem_buddy_set(struct vmem_buddy* buddy)
{
        if (buddy == NULL)
                return -E_NULL_PTR;
        if (buddy->system == NULL)
                return -E_NULL_PTR;

        size_t size = buddy->size >> 12;
        idx_t buddy_power = log2i(size);
        debug("Buddy going to size: 2^%i * 0x1000\n", buddy_power);
        buddy->prev = NULL;
        buddy->next = buddy->system->buddies[buddy_power];
        buddy->system->buddies[buddy_power] = buddy;

        return -E_SUCCESS;
}

/**
 * \fn  vmem_buddy_find_adjecent
 * \brief Find the adjecent merge candidates not allocated (if existant)
 * \param buddy
 * \brief The buddy to find the adjecents to
 * \return The first adjecent buddy
 */
struct vmem_buddy*
vmem_buddy_find_adjecent(struct vmem_buddy* buddy)
{
        if (buddy == NULL)
                return NULL;
        struct vmem_buddy_system* system = buddy->system;
        if (system == NULL)
                return NULL;

        size_t size = buddy->size >> 12;
        idx_t buddy_power = log2i(size);
        size_t merge_aligned = buddy->size + buddy->size;

        int high = (addr_t)buddy->ptr % merge_aligned;

        struct vmem_buddy* cariage = system->buddies[buddy_power];
        for (; cariage != NULL; cariage = cariage->next)
        {
                if (cariage->ptr + cariage->size == buddy->ptr && high)
                        return cariage;
                if (buddy->ptr + buddy->size == cariage->ptr && !(high))
                        return cariage;
        }
        return NULL;
}

/**
 * \fn vmem_buddy_split
 * \brief Use this function to split up a buddy entry
 * \param buddy
 * \brief The buddy to be split
 * \return The newly formed buddy
 */
struct vmem_buddy*
vmem_buddy_split(struct vmem_buddy* buddy)
{
        if (buddy == NULL)
                return NULL;
        if (buddy->size <= 0x1000 || buddy->size % PAGESIZE != 0)
                return NULL;

        struct vmem_buddy* new = kalloc(sizeof(*new));
        if (new == NULL)
                return NULL;
        memset(new, 0, sizeof(*new));

        if (vmem_buddy_purge(buddy) != -E_SUCCESS)
                goto err;

        buddy->size /= 2;
        new->size = buddy->size;
        new->ptr = buddy->ptr + buddy->size;
        new->system = buddy->system;

        if (vmem_buddy_set(buddy) != -E_SUCCESS)
                goto err;
        if (vmem_buddy_set(new) != -E_SUCCESS)
                goto err;

        return new;
err:
        buddy->size += buddy->size;
        buddy->next = new->next;
        memset(new, 0, sizeof(*new));
        kfree(new);
        new = NULL;
        return NULL;
}

/**
 * \todo Write a function that merges two buddies that are adjecent
 * \fn vmem_buddy_merge
 * \brief Use this to merge two adjecent buddies
 * \return The merged buddy
 */
struct vmem_buddy*
vmem_buddy_merge(struct vmem_buddy* a, struct vmem_buddy* b)
{
        if (a == NULL || b == NULL)
                return NULL;
        if (a->size != b->size)
                return NULL;
        if (a->ptr + a->size != b->ptr && b->ptr + b->size != a->ptr)
                return NULL;
        if (a->ptr == b->ptr || a == b)
                return NULL;

        if (b->ptr < a->ptr)
        {
                struct vmem_buddy* c = a;
                a = b;
                b = c;
        }
        a->size += a->size;
        if (b->prev != NULL)
                b->prev->next = b->next;
        if (b->next != NULL)
                b->next->prev = b->prev;
        memset(b, 0, sizeof(*b));
        kfree(b);
        return NULL;
}

int
vmem_buddy_mark(struct vmem_buddy* buddy)
{
        if (buddy == NULL)
                return -E_NULL_PTR;

        vmem_buddy_purge(buddy);
        buddy->next = buddy->system->allocated;
        buddy->system->allocated = buddy;

        return -E_SUCCESS;
}

int
vmem_buddy_unmark(struct vmem_buddy* buddy)
{
        if (buddy == NULL)
                return -E_NULL_PTR;

        buddy->prev->next = buddy->next;
        buddy->next->prev = buddy->prev;

        vmem_buddy_set(buddy);
        return -E_SUCCESS;
}

mutex_t vmem_buddy_lock = mutex_unlocked;

/**
 * \todo Write the allocation function
 */
void*
vmem_buddy_alloc(struct vmem_buddy_system* system, size_t size)
{
        if (system == NULL || size == 0)
                return NULL;
        idx_t buddy_power = log2i(size>>12);
        if (buddy_power > BUDDY_NO_POWERS)
                return NULL;

        mutex_lock(&vmem_buddy_lock);

        int i = buddy_power;
        for (; i < BUDDY_NO_POWERS; i++)
        {
                if (system->buddies[i] != NULL)
                        continue;
        }
        if (i > BUDDY_NO_POWERS)
                goto err;
        for (; i < buddy_power; i--)
        {
                vmem_buddy_split(system->buddies[i]);
        }
        struct vmem_buddy* c = system->buddies[i];
        vmem_buddy_mark(c);
        mutex_unlock(&vmem_buddy_lock);
        return c->ptr;

err:
        mutex_unlock(&vmem_buddy_lock);
        return NULL;
}

/**
 * \todo Write the freeing function
 */
void
vmem_buddy_free(struct vmem_buddy_system* system, void* ptr)
{
        if (ptr == NULL || system == NULL)
                return;
        mutex_lock(&vmem_buddy_lock);
        struct vmem_buddy* cariage = system->allocated;
        for(;cariage != NULL; cariage = cariage->next)
        {
                if (cariage->ptr == ptr)
                {
                        vmem_buddy_unmark(cariage);
                        struct vmem_buddy* adjecent = NULL;
                        while((adjecent = vmem_buddy_find_adjecent(cariage))
                                                                        != NULL)
                        {
                                cariage = vmem_buddy_merge(cariage, adjecent);
                        }
                        mutex_unlock(&vmem_buddy_lock);
                        return;
                }
        }
        mutex_unlock(&vmem_buddy_lock);
        return;
}

void
vmem_buddy_dump(struct vmem_buddy_system* system)
{
        if (system == NULL)
                return;

        struct vmem_buddy* cariage = system->allocated;
        debug("Allocated:\t");
        if (cariage == NULL)
                debug("Empty\n");
        else
        {
                debug("\n");
                for (;cariage != NULL; cariage = cariage->next)
                        debug("ptr: %X\tsize: %X\n", (int)cariage->ptr, cariage->size);
        }
        int i = 0;
        for (; i < BUDDY_NO_POWERS; i++)
        {
                cariage = system->buddies[i];
                debug("2^%i * 0x1000:\n", i);
                for (; cariage != NULL; cariage = cariage->next)
                        debug("ptr: %X\tsize: %X\n", (int)cariage->ptr, cariage->size);
        }
}

int
vmem_buddy_test()
{
        addr_t start_addr = (addr_t)&end;
        debug("end:  %X\t", start_addr);
        addr_t size = 0x80*PAGESIZE;
        if (start_addr % size != 0)
                start_addr += size - ((int)start_addr % size);
        debug("start_addr: %X\n", start_addr);

        void* start_ptr = (void*)start_addr;
        struct vmem_buddy_system* s = vmem_buddy_system_init(start_ptr, size);
        if (s == NULL)
                return -E_GENERIC;
        vmem_buddy_dump(s);
        return -E_SUCCESS;
}

/**
 * @}
 * \file
 */
