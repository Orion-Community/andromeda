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

/**
 * \todo Build a buddy system initialiser
 */
struct vmem_buddy_system*
vmem_buddy_system_init()
{
        return NULL;
}

/**
 * \todo Build a function that removes a buddy from its list
 */
int
vmem_buddy_purge(struct vmem_buddy* buddy, idx_t list)
{
        return -E_NOFUNCTION;
}

/**
 * \todo Build a function that inserts a buddy into a list
 */
int
vmem_buddy_set(struct vmem_buddy* buddy, idx_t list)
{
        return -E_NOFUNCTION;
}

/**
 * \todo Write a function that finds buddies adjecent to this one
 */
struct vmem_buddy*
find_adjecent(struct vmem_buddy* buddy)
{
        if (buddy == NULL)
                return NULL;
        struct vmem_buddy_system* system = buddy->system;
        if (system == NULL)
                return NULL;

        size_t size = buddy->size >> 12;
        idx_t buddy_power = log2i(size);
        struct vmem_buddy* cariage = system->buddies[buddy_power];
        for (; cariage != NULL; cariage = cariage->next)
        {
                if (cariage->ptr + cariage->size == buddy->ptr)
                        return cariage;
                if (buddy->ptr + buddy->size == cariage->ptr)
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

        buddy->size /= 2;
        new->size = buddy->size;
        new->ptr = buddy->ptr + buddy->size;
        new->prev = buddy;
        new->next = buddy->next;
        new->system = buddy->system;
        buddy->next = new;
        if (vmem_buddy_purge(buddy, log2i(buddy->size)+1) != -E_SUCCESS)
                goto err;
        if (vmem_buddy_purge(new, log2i(new->size)+1) != -E_SUCCESS)
                goto err;
        if (vmem_buddy_set(buddy, log2i(buddy->size)) != -E_SUCCESS)
                goto err;
        if (vmem_buddy_set(new, log2i(new->size)) != -E_SUCCESS)
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

/**
 * \todo Write the allocation function
 */
void*
vmem_buddy_alloc(struct vmem_buddy_system* system, size_t size)
{
        return NULL;
}

/**
 * \todo Write the freeing function
 */
void
vmem_buddy_free(struct vmem_buddy_system* system, void* ptr)
{
}

/**
 * @}
 * \file
 */
