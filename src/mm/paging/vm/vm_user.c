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

#include <andromeda/error.h>
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
        s->free = kalloc(sizeof(*s->free));
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
 * \fn vm_segment_mark_allocated
 * \param segment
 * \param range
 * \return A standard error code
 */
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

static int
vm_segment_mark_free(segment, range)
struct vm_segment* segment;
struct vm_range_descriptor* range;
{
        if (segment == NULL || range == NULL)
                return -E_NULL_PTR;

        /* Dislodge the range and restore the order of the list */
        if (range->prev == NULL)
                segment->allocated = range->next;
        else
                range->prev->next = range->next;
        if (range->next != NULL)
                range->next->prev = range->prev;

        /* Put this range at the start of the free list */
        range->prev = NULL;
        range->next = segment->free;
        if (segment->free != NULL)
                segment->free->prev = range;
        segment->free = range;

        return -E_SUCCESS;
}

static inline int
vm_range_remove_node(range)
struct vm_range_descriptor* range;
{
        if (range == NULL)
                return -E_NULL_PTR;

        if (range->next != NULL)
                range->next->prev = range->prev;

        if (range->prev != NULL)
                range->prev->next = range->next;

        else if (range->parent->free == range)
                range->parent->free = range->next;

        else
                range->parent->allocated = range->next;

        return kfree(range);
}

static int
vm_segment_compress_ranges(segment, range)
struct vm_segment* segment;
struct vm_range_descriptor* range;
{
        if (segment == NULL || range == NULL)
                return -E_NULL_PTR;

        /* Loop through the list to find connecting ranges */
        struct vm_range_descriptor* x = segment->free;
        while (x != NULL)
        {
                if (x->base + x->size == range->base)
                {
                        /* Consume information in the carriage */
                        range->base = x->base;
                        range->size = x->size;

                        /* And take the node out of the collection */
                        vm_range_remove_node(x);
                }
                else if (range->base + range->size == x->base)
                {
                        /* Consume information in the carriage */
                        range->size += x->size;

                        /* And take the node out of the collection */
                        vm_range_remove_node(x);
                }
                x = x->next;
        }
        return -E_SUCCESS;
}

/**
 * \fn vm_segment_free
 * \brief Clear the page range up for allocation.
 * \param s
 * \brief The segment to work with
 * \return A standard error code
 */
int vm_segment_free(struct vm_segment* s, void* ptr)
{
        if (s == NULL || ptr == NULL)
                return -E_NULL_PTR;

        mutex_lock(&s->lock);

        /* Find the range associated with this pointer */
        struct vm_range_descriptor* x = s->allocated;
        while (x != NULL && x->base != ptr)
                x = x->next;

        /* If nothing was found, the argument was wrong */
        if (x == NULL)
        {
                mutex_unlock(&s->lock);
                return -E_INVALID_ARG;
        }

        /* Take the descriptor out and put it back into the free list */
        vm_segment_mark_free(s, x);

        /* Now get the physical page, if mapped and free that up if possible */
        void* phys = vm_get_phys(x);
        if (phys != NULL)
        {
                page_free(phys);
        }

        /*
         * Now use this descriptor to find a predecessor and successor. If there
         * is one, merge with it, leaving the pointer to x intact. Otherwise,
         * just return.
         */
        vm_segment_compress_ranges(s, x);

        mutex_unlock(&s->lock);
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

static int
vm_range_split(struct vm_range_descriptor* src, size_t size)
{
        if (src == NULL || size == 0)
                return -E_NULL_PTR;

        if (src->size == size)
                return -E_SUCCESS;

        /* Create a new descritor to keep track of the other bit of memory */
        struct vm_range_descriptor* tmp = kalloc(sizeof(*tmp));
        if (tmp == NULL)
                return -E_NULL_PTR;

        /* Basic intialisation */
        memset(tmp, 0, sizeof(*tmp));

        /* Configure new descriptor */
        tmp->size = src->size-size;
        tmp->base = src->base+size;
        tmp->parent = src->parent;
        /* And resize the original descriptor */
        src->size = size;

        /* Add the new descriptor into the list */
        tmp->next = src->next;
        tmp->next->prev = tmp;
        tmp->prev = src;
        src->next = tmp;

        /* And we're done */
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

        /*
         * If the size is not page allocation aligned, align it by rounding up.
         * It is much easier to just give out ranges of physical page allocation
         * size. This is because if we don't do this, more effort has to be made
         * keeping track of the number of references to the physically allocated
         * range.
         */
        if (size % PAGE_ALLOC_FACTOR != 0)
                size += PAGE_ALLOC_FACTOR - size % PAGE_ALLOC_FACTOR;

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

static struct vm_segment*
vm_find_segment(char* name)
{
        if (name == NULL)
                return NULL;

        int len = strlen(name);
        struct vm_segment* i = (&vm_core)->segments;
        while (i != NULL)
        {
                if (i->name == NULL)
                        goto next;
                int ilen = strlen(i->name);
                if (ilen == len)
                {
                        if (memcmp(i->name, name, len) == 0)
                                return i;
                }

next:
                i = i->next;
        }
        return NULL;
}

void*
vm_get_kernel_heap_pages(size_t size)
{
        if (size == 0)
                return NULL;

        if (size % PAGE_ALLOC_FACTOR != 0)
                size += PAGE_ALLOC_FACTOR - size % PAGE_ALLOC_FACTOR;

        struct vm_segment* heap = vm_find_segment(".heap");
        if (heap == NULL)
                return NULL;

        return vm_segment_alloc(heap, size);
}

int vm_free_kernel_heap_pages(void* ptr)
{
        if (ptr == NULL)
                return -E_NULL_PTR;

        struct vm_segment* heap = vm_find_segment(".heap");
        if (heap == NULL)
                return -E_HEAP_GENERIC;

        return vm_segment_free(heap, ptr);
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
        vm_dump(&vm_core);

        struct vm_segment* heap = vm_find_segment(".heap");
        printf("Heap segment: %X\n", (int)heap);
        if (heap == NULL)
                return -E_NULL_PTR;

        printf("Free ranges:\n");
        vm_dump_ranges(heap->free);
        printf("Allocated ranges:\n");
        vm_dump_ranges(heap->allocated);

        void* tst = vm_get_kernel_heap_pages(0x10000);

        demand_key();
        printf("\nAfter allocation:\n");
        printf("Allocated: %X\n", (int)tst);
        printf("Free ranges:\n");
        vm_dump_ranges(heap->free);
        printf("Allocated ranges:\n");
        vm_dump_ranges(heap->allocated);

        vm_free_kernel_heap_pages(tst);

        demand_key();
        printf("\nAfter freeing:\n");
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
