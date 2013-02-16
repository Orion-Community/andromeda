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
#include <types.h>

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
vm_new(int pid)
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
 * \param size
 * \brief The size of the newly allocated segment
 * \param p
 * \brief The descriptor to put the segment into
 * \return the segment which is added to the descriptor
 */
struct vm_segment*
vm_new_segment(void* virt, size_t size, struct vm_descriptor* p)
{
        struct vm_segment* s = kalloc(sizeof(*s));
        if (s == NULL)
                return NULL;

        memset(s, 0, sizeof(*s));
        s->virt_base = virt;
        s->size = size;

        /* Add allocation data in here */
        s->allocated = NULL;
        s->free = kalloc(sizeof(*s->free));
        if (s->free == NULL)
                goto err;
        memset(s->free, 0, sizeof(*s->free));
        s->free->base = s->virt_base;
        s->free->size = s->size;

        /* Add the segment into the list, but only if all is well */
        s->next = p->segments;
        if (s->next != NULL)
                s->next->prev = s;
        p->segments = s;

        return s;
err:
        kfree(s);
        return NULL;
}

/**
 * \fn vm_free_page
 * \brief Clear the page up for allocation.
 * \param s
 * \brief The segment to work with
 * \return A standard error code
 */
int vm_segment_free(struct vm_segment* s, void* ptr)
{
        /**
         * \todo Iterate through page tables to find allocated pages
         * \todo Free up those allocated pages
         */

        return -E_NOFUNCTION;
}

/**
 * \fn vm_segment_grow
 * \brief Grow the size of the segment
 * \param s
 * \param size
 * \brief The size by which we want the segment to grow
 * \return A standard error code
 * By the way, notice how it is only possible to grow a segment?
 * Indeed that's by design. Segments can only grow, not shrink. If they could,
 * they might cut into allocated data, and that's bad. If pages aren't in use
 * anymore, paging should take care of those, so no need to worry about it here.
 *
 * \todo implement swapping and destroying of pages that have gone out of use.
 */
int vm_segment_grow(struct vm_segment* s, size_t size)
{
        return -E_NOFUNCTION;
}

static int
vm_range_split(struct vm_range_descriptor* src, size_t size)
{
        if (src == NULL || size == 0)
                return -E_NULL_PTR;

        if (src->size == size)
                return -E_SUCCESS;

        struct vm_range_descriptor* tmp = kalloc(sizeof(*tmp));
        if (tmp == NULL)
                return -E_NULL_PTR;

        memset(tmp, 0, sizeof(*tmp));

        tmp->size = src->size-size;
        tmp->base = src->base+size;
        src->size = size;

        tmp->next = src->next;
        tmp->next->prev = tmp;
        tmp->prev = src;
        src->next = tmp;

        return -E_SUCCESS;
}

static int
vm_segment_mark_allocated(segment, range)
struct vm_segment* segment;
struct vm_range_descriptor* range;
{
        if (segment == NULL || range == NULL)
                return -E_NULL_PTR;

        /* Disconnect the node from the free list, maybe is a bit messy */
        if (range->prev == NULL)
                segment->free = range->next;
        else
                range->prev->next = range->next;
        if (range->next != NULL)
                range->next->prev = range->prev;

        /* And add the range into the allocated list. */
        range->prev = NULL;
        range->next = segment->allocated;
        if (range->next != NULL)
                range->next->prev = range;
        segment->allocated = range;

        return -E_SUCCESS;
}

/**
 * \fn vm_segment_alloc
 * \brief Allocate a number of pages from the segment.
 * \param s
 * \param size
 * \brief Size in number of bytes, rounded to page size
 */
void* vm_segment_alloc(struct vm_segment *s, size_t size)
{
        if (s == NULL || size == 0 || s->free == NULL)
                return NULL;

        /* Size has to be page aligned, in good integer style, round up */
        if (size % PAGE_SIZE != 0)
                size += PAGE_SIZE - size % PAGE_SIZE;

        /* There's a mutex here, don't forget to unlock */
        mutex_lock(&s->lock);

        /*
         * Let's try a best fit allocator here. It might be a little slower,
         * but memory space efficiency it might actually pay off here.
         */
        struct vm_range_descriptor* x = s->free;
        struct vm_range_descriptor* tmp = NULL;
        void* ret = NULL;
        for (; x != NULL; x = x->next)
        {
                /*
                 * If x matches the size requirement AND if x is a better fit
                 * than what we found before, mark this as our new best fit.
                 */
                if (x->size >= size && (tmp == NULL || tmp->size < x->size))
                        tmp = x;
        }
        /* If we didn't find anything we have no option but returning */
        if (tmp == NULL)
                goto err;

        /* If the range needs to be split up, do so here */
        vm_range_split(tmp, size);
        /* And mark our range as allocated */
        vm_segment_mark_allocated(s, tmp);

        /*
         * All went well and now we can use the pointer in the range to return
         * a result.
         */
        ret = tmp->base;
err:    /*
         * Even if stuff went wrong, we want it to go past here, in order to
         * clean the mutex up, so that the system doesn't hang on future
         * attempts.
         */
        mutex_unlock(&s->lock);

        return ret;
}

/**
 * \fn vm_segment_map
 * \brief Map a physical location into a segment
 * \param s
 * \brief The segment to map into
 * \param p
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
 * \fn vm_segment_clean
 * \brief Remove the segment in question.
 * \param s
 * \return
 */
int
vm_segment_clean(struct vm_segment* s)
{
        if (s == NULL)
                return -E_NULL_PTR;

        struct vm_range_descriptor* x = s->free->next;
        struct vm_range_descriptor* xx = s->free;
itterate:
        for (; xx != NULL; xx = x, x = x->next)
        {
                kfree(xx);
        }
        if (s->allocated != NULL)
        {
                x = s->allocated->next;
                xx = s->allocated;
                goto itterate;
        }
        if (s->prev != NULL)
                s->prev->next = s->next;
        if (s->next != NULL)
                s->next->prev = s->prev;

        if (s->pages != NULL)
                kfree(s->pages);

        kfree(s);

        return -E_SUCCESS;
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
                if (vm_segment_clean(this) != -E_SUCCESS)
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
                        return (void*)x86_pte_get_phys(virt, carriage);
                }
        }
#endif
        return x86_pte_get_phys(virt);
}

/**
 * \fn vm_load_task
 * \brief Load in the virtual memory context of the given task
 * \return A standard error code
 */
int
vm_load_task()
{
        return -E_NOFUNCTION;
}

/**
 * \fn vm_unload_task
 * \brief Disable access to the pages owned by this task
 * \return
 */
int
vm_unload_task()
{
        return -E_NOFUNCTION;
}

/**
 * @}
 * \file
 */
