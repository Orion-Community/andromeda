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
 * \fn vm_segment_mark_allocated
 * \param segment
 * \param range
 * \return A standard error code
 */
static int
vm_range_mark_allocated(segment, range)
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
vm_range_mark_mapped(segment, range)
struct vm_segment* segment;
struct vm_range_descriptor* range;
{
        if (range == NULL || segment == NULL)
                return -E_NULL_PTR;

        /*
         * We're assuming that the range is previously allocated. Let's take it
         * out of the list.
         */
        if (range->prev == NULL)
        {
                if (segment->allocated == range)
                        segment->allocated = range->next;
                else
                        return -E_INVALID_ARG;
        }
        else
                        range->prev->next = range->next;

        if (range->next != NULL)
                range->next->prev = range->prev;

        /* Now stuff the range into the mapped list */
        range->prev = NULL;
        range->next = segment->mapped;
        if (segment->mapped != NULL)
                segment->mapped->prev = range;
        segment->mapped = range;

        /* Done */
        return -E_SUCCESS;
}

static int
vm_range_mark_unmapped(segment, range)
struct vm_segment* segment;
struct vm_range_descriptor* range;
{
        if (segment == NULL || range == NULL)
                return -E_NULL_PTR;

        /*
         * We're assuming that the range is previously allocated. Let's take it
         * out of the list.
         */
        if (range->prev == NULL)
        {
                if (segment->mapped == range)
                        segment->mapped = range->next;
                else
                        return -E_INVALID_ARG;
        }
        else
                        range->prev->next = range->next;

        if (range->next != NULL)
                range->next->prev = range->prev;

        /* Now stuff the range into the mapped list */
        range->prev = NULL;
        range->next = segment->allocated;
        if (segment->allocated != NULL)
                segment->allocated->prev = range;
        segment->allocated = range;

        return -E_NOFUNCTION;
}

static int
vm_range_mark_free(segment, range)
struct vm_segment* segment;
struct vm_range_descriptor* range;
{
        if (segment == NULL || range == NULL)
                return -E_NULL_PTR;

        /* Dislodge the range and restore the order of the list */
        if (range->prev == NULL)
        {
                if (segment->allocated == range)
                        segment->allocated = range->next;
                else
                        return -E_INVALID_ARG;
        }
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
                        range->size += x->size;

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
        vm_range_mark_free(s, x);

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
 * \fn vm_range_split
 * \param src
 * \param size
 * \return Standard error code
 */
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
        vm_range_mark_allocated(s, tmp);

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
 * \fn vm_map
 * \param virt
 * \param phys
 * \param s
 * \return virtual address of mapped range
 */
void* vm_map(void* virt, void* phys, struct vm_segment* s)
{
        if (virt == NULL || phys == NULL || s == NULL)
                return NULL;

        mutex_lock(&s->lock);
        struct vm_range_descriptor* r = s->allocated;
        for(; r != NULL; r = r->next)
        {
                if (r->base == virt)
                        break;
        }

        if (r == NULL)
                goto err;

        vm_range_mark_mapped(s, r);

        size_t cnt = r->size;
        addr_t v = (addr_t)virt;
        addr_t p = (addr_t)phys;
        int i = 0;
        for(; i < cnt; i += PAGE_ALLOC_FACTOR)
        {
                if (page_claim(p + i) == NULL)
                        goto gofixit;
                x86_pte_map(v + i, p + i, 0);
        }

err:
        mutex_unlock(&s->lock);

        return (r == NULL) ? NULL : r->base;

gofixit:
        for (; i >= 0; i-= PAGE_ALLOC_FACTOR)
        {
                x86_pte_unset_page(v + i);
                page_free((void*)p + i);
        }
        mutex_unlock(&s->lock);
        return NULL;
}

/**
 * \fn vm_unmap
 * \param virt
 * \param s
 * \return Standard error code
 */
int vm_unmap(void* virt, struct vm_segment* s)
{
        if (virt == NULL || s == NULL)
                return -E_NULL_PTR;

        mutex_lock(&s->lock);
        struct vm_range_descriptor* r = s->mapped;
        for (; r != NULL; r = r->next)
        {
                if (r->base == virt)
                        break;
        }
        if (r == NULL)
                goto err;

        void* p = vm_get_phys(virt);
        if (p == NULL)
                goto err;

        int i = 0;
        for (;i < r->size; i += PAGE_ALLOC_FACTOR)
        {
                x86_pte_unset_page(virt + i);
                page_free(p + i);
        }

        vm_range_mark_unmapped(s, r);
err:
        mutex_unlock(&s->lock);

        return (r == NULL) ? -E_NULL_PTR : -E_SUCCESS;
}

/**
 * \fn vm_find_segment
 * \param name
 * \return The requested segment or 0
 */
#ifndef VM_DBG
static struct vm_segment*
#else
struct vm_segment*
#endif
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

/**
 * \fn vm_free_kernel_heap_pages
 * \param ptr
 * \return
 */
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
 * \fn vm_get_kernel_heap_pages
 * \param size
 */
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

/**
 * \fn vm_map_heap
 * \param phys
 * \param size
 * \return allocated and mapped virtual pointer
 */
void*
vm_map_heap(void* phys, size_t size)
{
        if (phys == NULL || size == 0)
                return NULL;

        struct vm_segment* heap = vm_find_segment(".heap");
        if (heap == NULL)
                return NULL;

        void* virt = vm_segment_alloc(heap, size);
        if (vm_map(virt, phys, heap) != virt)
        {
                vm_segment_free(heap, virt);
                return NULL;
        }

        return virt;
}

/**
 * \fn vm_unmap_heap
 * \param virt
 * \return Generic error code
 */
int
vm_unmap_heap(void* virt)
{
        if (virt == NULL)
                return -E_NULL_PTR;

        struct vm_segment* heap = vm_find_segment(".heap");
        if (heap == NULL)
                return -E_HEAP_GENERIC;

        int ret = vm_unmap(virt, heap);
        ret |= vm_segment_free(heap, virt);
        return ret;
}

/**
 * @} \file
 */
