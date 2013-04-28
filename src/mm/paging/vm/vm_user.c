/*
 * Andromeda
 * Copyright (C) 2012 - 2013  Bart Kuivenhoven
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

#include <andromeda/error.h>
#include <andromeda/system.h>
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
vm_new(unsigned int pid)
{
        struct vm_descriptor* p = kmalloc(sizeof(*p));
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
        struct vm_segment* s = kmalloc(sizeof(*s));
        if (s == NULL)
                return NULL;

        /*
         * Segment size can only be PAGE_ALLOC_FACTOR aligned. If it isn't it
         * will mess up once physical pages get allocated to it.
         */
        if (size % PAGE_ALLOC_FACTOR != 0)
                size += PAGE_ALLOC_FACTOR - size % PAGE_ALLOC_FACTOR;

        memset(s, 0, sizeof(*s));
        s->virt_base = virt;
        s->size = size;

        /* Add allocation data in here */
        s->allocated = NULL;
        s->free = kmalloc(sizeof(*s->free));
        if (s->free == NULL)
                goto err;
        memset(s->free, 0, sizeof(*s->free));
        s->free->base = s->virt_base;
        s->free->size = s->size;
        s->free->parent = s;

        s->parent = p;

        /* Add the segment into the list, but only if all is well */
        mutex_lock(&p->lock);
        if (p->segments != NULL)
        {
                mutex_lock(&p->segments->lock);
                s->next = p->segments;
                p->segments->prev = s;
                mutex_unlock(&s->next->lock);
        }
        p->segments = s;
        mutex_unlock(&p->lock);

        return s;
err:
        kfree(s);
        return NULL;
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
        if (s == NULL || size <= 0 || s->parent == NULL)
                return -E_INVALID_ARG;

        /* Round size up if necessary */
        if (size % PAGE_ALLOC_FACTOR != 0)
                size += PAGE_ALLOC_FACTOR - size % PAGE_ALLOC_FACTOR;

        struct vm_descriptor* d = s->parent;

retry:
        /* This stuff is all critical ... */
        mutex_lock(&d->lock);

        int ns = s->size + size;
        addr_t nend = (addr_t)s->virt_base + ns;

        int ret = -E_OUTOFBOUNDS;
        struct vm_segment* i;

        /* Basically what we don't want is for our segments to overlap */
        for (i = d->segments; i != NULL; i = i->next)
        {
                /* If we're verifying with our selves we are doing it wrong */
                if (i == s)
                        continue;

                addr_t start;
                addr_t end;
                if (mutex_test(&i->lock) == 0)
                {
                        start = (addr_t)i->virt_base;
                        end = (addr_t)(i->virt_base + i->size);
                }
                else
                {
                        mutex_unlock(&d->lock);
                        /** \todo Find a way to yield CPU time here */
                        goto retry;
                }

                /*
                 * Do some overlapping checks
                 */
                if ((nend > start && nend <= end) || (ns >= start && ns < end))
                        goto err;

                if (ns <= start && nend >= end)
                        goto err;
        }

        /*
         * Since we're not going to overlap, we can now complete the
         * transaction. Also set the return value to success.
         */
        mutex_lock(&s->lock);
        s->size += size;
        mutex_unlock(&s->lock);
        ret = -E_SUCCESS;
err:
        mutex_unlock(&d->lock);

        return ret;
}

/**
 * \fn vm_segment_load
 * \brief Map a physical location into a segment
 * \param s
 * \brief The segment to map into
 * \param p
 * \brief The pages to map
 * \return A standard error code
 */
int
vm_segment_load(struct vm_segment* s, struct mm_page_descriptor* p)
{
        /**
         * \TODO: If pte_descriptor == loaded, map segment into page directory
         */
        return -E_NOFUNCTION;
}

int
vm_segment_unload(struct vm_segment* s, struct mm_page_descriptor* p)
{
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

        mutex_lock(&s->next->lock);
        mutex_lock(&s->lock);
        mutex_lock(&s->prev->lock);

        /* Take this segment out of the list */
        if (s->prev != NULL)
                s->prev->next = s->next;
        else
                s->parent->segments = s->next;

        if (s->next != NULL)
                s->next->prev = s->prev;

        mutex_unlock(&s->next->lock);
        mutex_unlock(&s->prev->lock);
        /*
         *  We're leaving this segment in a locked state, as it is going to be
         *  thrown away anyway.
         */

        /* Free up the free memory descriptors */
        struct vm_range_descriptor* x = s->free->next;
        struct vm_range_descriptor* xx = s->free;
        s->free = NULL;
itterate:
        for (; xx != NULL; xx = x, x = x->next)
        {
                /* If these ranges are allocated physically, free them up */
                kfree(xx);
        }
        /* Now do the same for allocated memory descriptors */
        if (s->allocated != NULL)
        {
                x = s->allocated->next;
                xx = s->allocated;
                s->allocated = NULL;
                goto itterate;
        }

        /* If we still have physical pages, clean those up */
        if (s->pages != NULL)
                kfree(s->pages);

        /* Remove this segment*/
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
        /* Lock it, even though it won't get unlocked */
        mutex_lock(&p->lock);
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
vm_get_phys(void* virt)
{
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

#ifdef VM_DBG
int vm_dump_ranges(struct vm_range_descriptor* r)
{
        if (r == NULL)
                return -E_NULL_PTR;

        printf("this: %X\tnext: %X\tprev: %X\n", r, r->next, r->prev);
        printf("base: %X\n", r->base);
        printf("size: %X\n", r->size);

        vm_dump_ranges(r->next);
        return -E_SUCCESS;
}

int vm_dump_segments(struct vm_segment* s)
{
        if (s == NULL)
                return -E_NULL_PTR;

        printf("Segment: %s\n", s->name);
        printf("virt: %X\n", s->virt_base);
        printf("size: %X\n", s->size);

        printf("\nFree:\n");
        demand_key();

        mutex_lock(&s->lock);
        struct vm_range_descriptor* free = s->free;

        vm_dump_ranges(free);
        printf("Allocated:\n");
        mutex_unlock(&s->lock);

        demand_key();
        mutex_lock(&s->lock);
        struct vm_range_descriptor* allocated = s->allocated;
        vm_dump_ranges(allocated);

        struct vm_segment* sn = s->next;
        mutex_unlock(&s->lock);

        printf("next segment: %X\n", sn);
        demand_key();

        vm_dump_segments(sn);
        return -E_SUCCESS;
}

int vm_dump(struct vm_descriptor* v)
{
        if (v == NULL)
                return -E_NULL_PTR;

        if (v->name != NULL)
                printf("vm_descriptor name: %s\n", v->name);
        printf("cpl: %X\npid: %X\n", v->cpl, v->pid);
        printf("Segments: \n");
        mutex_lock(&v->lock);
        vm_dump_segments(v->segments);
        mutex_unlock(&v->lock);

        return -E_SUCCESS;
}

int vm_test()
{
        //vm_dump(&vm_core);

        struct vm_segment* heap = vm_find_segment(".heap");
        printf("Heap segment: %X\n", (int)heap);
        if (heap == NULL)
                return -E_NULL_PTR;

        printf("Free ranges:\n");
        vm_dump_ranges(heap->free);
        printf("Allocated ranges:\n");
        vm_dump_ranges(heap->allocated);

        demand_key();
        void* tst = vm_get_kernel_heap_pages(0x1000);
        void* tst2 = vm_get_kernel_heap_pages(0xb1aa7);

        printf("\nAfter allocation:\n");
        printf("Allocated: %X\n", (int)tst);
        printf("Free ranges: %X\n", heap->free);
        vm_dump_ranges(heap->free);
        printf("Allocated ranges: %X\n", heap->allocated);
        vm_dump_ranges(heap->allocated);

        demand_key();
        vm_free_kernel_heap_pages(tst2);
        printf("\nAfter freeing the first:\n");
        printf("Allocated: %X\n", (int)tst);
        printf("Free ranges: %X\n", heap->free);
        vm_dump_ranges(heap->free);
        printf("Allocated ranges: %X\n", heap->allocated);
        vm_dump_ranges(heap->allocated);

        demand_key();
        vm_free_kernel_heap_pages(tst);
        printf("\nAfter freeing all:\n");
        printf("Allocated: %X\n", (int)tst);
        printf("Free ranges:\n");
        vm_dump_ranges(heap->free);
        printf("Allocated ranges:\n");
}
#endif

/**
 * @}
 * \file
 */
