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
 * \addtogroup VM
 * @{
 */

int vm_segment_mark_loaded_global(struct vm_segment* s)
{
        idx_t i = 0;
        for (; i < CPU_LIMIT; i++)
                vm_segment_mark_loaded(i, s);
        return -E_SUCCESS;
}

int vm_segment_mark_loaded(int cpuid, struct vm_segment* s)
{
        if (vm_loaded[cpuid]->find((int)s->virt_base, vm_loaded[cpuid]) != NULL)
                return -E_ALREADY_INITIALISED;

        if (vm_loaded[cpuid]->add((int)s->virt_base, s,
                        vm_loaded[cpuid]) != -E_SUCCESS)
                return -E_GENERIC;
        return -E_SUCCESS;
}

int vm_segment_mark_unloaded(int cpuid, struct vm_segment* s)
{
        if (vm_loaded[cpuid]->find((int)s->virt_base, vm_loaded[cpuid]) == NULL)
                return -E_NOT_YET_INITIALISED;

        if (vm_loaded[cpuid]->delete((int)s->virt_base,
                        vm_loaded[cpuid]) != -E_SUCCESS) {
                return -E_GENERIC;
        }

        return -E_SUCCESS;
}

/**
 * \fn vm_get_loaded
 * \param cpuid
 * \param addr
 * \return The segment that currently has this address mapped
 */
static inline struct vm_segment*
vm_get_loaded(int cpuid, void* addr)
{
        addr_t a = (addr_t)addr;
        a &= ~0x3FF;

        struct tree* tree = vm_loaded[cpuid]->find_close((int)addr,
                        vm_loaded[cpuid]);

        struct vm_segment* segment = tree->data;
        boolean go_back = FALSE;
        boolean go_fwd = FALSE;
        while (TRUE) {
                addr_t seg_base = (addr_t)segment->virt_base;
                addr_t seg_end = (addr_t)segment->virt_base + segment->size;
                if (seg_base <= a && a < seg_end)
                        return segment;

                if (seg_base < a) {
                        go_back = TRUE;
                        tree = tree->prev;
                }
                if (seg_end > a) {
                        go_fwd = TRUE;
                        tree = tree->next;
                }

                if (tree == NULL)
                        return NULL ;

                if (go_fwd == TRUE && go_back == TRUE)
                        return NULL ;

                segment = tree->data;
        }
        return NULL ;
}

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
                return NULL ;

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
                return NULL ;

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

        s->pages = kmalloc(sizeof(*s->pages));
        if (s->pages == NULL)
                goto err1;
        memset(s->pages, 0, sizeof(*s->pages));
        s->pages->size = s->size;
        s->pages->virt = s->virt_base;

        s->parent = p;

        /* Add the segment into the list, but only if all is well */
        mutex_lock(&p->lock);
        if (p->segments != NULL) {
                mutex_lock(&p->segments->lock);
                s->next = p->segments;
                p->segments->prev = s;
                mutex_unlock(&s->next->lock);
        }
        p->segments = s;
        mutex_unlock(&p->lock);

        return s;
        err1:
        kfree(s->free);
        err:
        kfree(s);
        return NULL ;
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

        addr_t ns = s->size + size;
        addr_t nend = (addr_t)s->virt_base + ns;

        int ret = -E_OUTOFBOUNDS;
        struct vm_segment* i;

        /* Basically what we don't want is for our segments to overlap */
        for (i = d->segments; i != NULL ; i = i->next) {
                /* If we're verifying with our selves we are doing it wrong */
                if (i == s)
                        continue;

                addr_t start;
                addr_t end;
                if (mutex_test(&i->lock) == 0) {
                        start = (addr_t)i->virt_base;
                        end = (addr_t)(i->virt_base + i->size);
                } else {
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
        err: mutex_unlock(&d->lock);

        return ret;
}

/**
 * \fn vm_segment_load
 * \brief Map a physical location into a segment
 * \param s
 * \brief The segment to map into
 * \return A standard error code
 */
int vm_segment_load(int cpu, struct vm_segment* s)
{
        if (s == NULL || s->pages == NULL) {
                return -E_NULL_PTR;
        }

        int ret = -E_SUCCESS;
        if (vm_loaded[cpu]->find((int)s->virt_base, vm_loaded[cpu]) == NULL) {
                ret = page_map_range(cpu, s->pages);
                if (ret == -E_SUCCESS) {
                        ret = vm_segment_mark_loaded(cpu, s);
                } else {
                        return ret;
                }
        } else {
                return -E_ALREADY_INITIALISED;
        }
        return ret;
}

int vm_segment_unload(int cpu, struct vm_segment* s)
{
        if (s == NULL || s->pages == NULL) {
                return -E_INVALID_ARG;
        }
        int ret = page_unmap_range(cpu, s->pages);
        if (ret == -E_SUCCESS) {
                ret = vm_segment_mark_unloaded(cpu, s);
        } else {
                return -ret;
        }
        return ret;
}

/**
 * \fn vm_segment_clean
 * \brief Remove the segment in question.
 * \param s
 * \return
 */
int vm_segment_clean(struct vm_segment* s)
{
        if (s == NULL)
                return -E_NULL_PTR;

        if (s->next != 0)
                mutex_lock(&s->next->lock);
        mutex_lock(&s->lock);
        if (s->prev != NULL)
                mutex_lock(&s->prev->lock);

        /* Take this segment out of the list */
        if (s->prev != NULL)
                s->prev->next = s->next;
        else
                s->parent->segments = s->next;

        if (s->next != NULL)
                s->next->prev = s->prev;

        if (s->next != NULL)
                mutex_unlock(&s->next->lock);
        if (s->prev != NULL)
                mutex_unlock(&s->prev->lock);
        /*
         *  We're leaving this segment in a locked state, as it is going to be
         *  thrown away anyway.
         */

        /* Free up the free memory descriptors */
        struct vm_range_descriptor* x = s->free->next;
        struct vm_range_descriptor* xx = s->free;
        s->free = NULL;
        itterate: while (x != NULL ) {
                xx = x;
                x = x->next;
                /* If these ranges are allocated physically, free them up */
                kfree(xx);
        }
        /* Now do the same for allocated memory descriptors */
        if (s->allocated != NULL) {
                x = s->allocated->next;
                xx = s->allocated;
                s->allocated = NULL;
                goto itterate;
        }

        /* If we still have physical pages, clean those up */
        if (s->pages != NULL) {
                page_range_cleanup(0, s->pages);
                kfree(s->pages);
                s->pages = NULL;
        }

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
int vm_free(struct vm_descriptor* p)
{
        /* Lock it, even though it won't get unlocked */
        mutex_lock(&p->lock);
        struct vm_segment* this = p->segments;
        struct vm_segment* next = this->next;

        while (this != NULL ) {
                if (vm_segment_clean(this) != -E_SUCCESS) {
                        /*
                         * We have just created a ghost task.
                         * For some reason, or another, this task we can't get
                         * rid of.
                         */
                        printf("Generic error!\n");
                        p->segments = next;
                        return -E_GENERIC;
                }
                this = next;
                if (next != NULL)
                        next = next->next;
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
vm_get_phys(int cpu, void* virt)
{
        return get_phys(cpu, virt);
}

/**
 * \fn vm_load_task
 * \brief Load in the virtual memory context of the given task
 * \return A standard error code
 */
int vm_load_task(int cpu, struct vm_descriptor* task)
{
        if (cpu >= CPU_LIMIT || task == NULL)
                return -E_INVALID_ARG;

        if (task->segments == NULL)
                return -E_NULL_PTR;

        struct vm_segment* runner = task->segments;
        while (runner != NULL ) {
                vm_segment_load(cpu, runner);

                runner = runner->next;
        }

        return -E_SUCCESS;
}

#if 0
static inline int
vm_range_cleanup(struct sys_mmu_range* range)
{
        if (range == NULL)
        return -E_NULL_PTR;

        struct sys_mmu_range_phys* runner = range->phys;
        struct sys_mmu_range_phys* next = range->phys->next;
        while (runner != NULL)
        {
                kfree(runner);
                runner = next;
                next = next->next;
        }
        kfree(range);

        return -E_SUCCESS;
}
#endif
/**
 * \fn vm_unload_task
 * \brief Disable access to the pages owned by this task
 * \return
 */
int vm_unload_task(int cpu, struct vm_descriptor* task)
{
        if (cpu >= CPU_LIMIT || task == NULL)
                return -E_INVALID_ARG;

        if (task->segments == NULL)
                return -E_NULL_PTR;

        struct vm_segment* runner = task->segments;
        while (runner != NULL ) {
                if (vm_segment_unload(cpu, runner) != -E_SUCCESS)
                        goto error;
                runner = runner->next;
        }

        return -E_SUCCESS;
error:
        panic("Something went terribly wrong in unloading the task!");
        return -E_GENERIC; /* Return statement to keep the compiler happy! */
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

int vm_user_fault_write(addr_t fault_addr, int mapped)
{
        panic("User space page faults currently remain unhandled");
        return -E_NOFUNCTION;
}

int vm_kernel_fault_write(addr_t fault_addr, int mapped)
{
        if (mapped) {
                printf(
                                "We don't do mapped pagefaults ... We just don't do them\n");
                goto problem;
        }

        /**
         * \todo Add permission checking
         */
        struct vm_segment* segment = vm_get_loaded(0, (void*)fault_addr);
        if (segment == NULL) {
                printf("Fault addr: %X\n", (uint32_t)fault_addr);
                panic("Trying to map an unloaded page!!!!!!");
                /**
                 * If this was in userspace, segfault!
                 */
        }

        void* phys = get_phys(0, (void*)(fault_addr & ~0x3FF));
        if (phys != NULL) {
                panic("Faulting on existing page ... wtf!");
        }

        phys = page_alloc();
        if (phys == NULL)
                panic("Out of memory!!!");

        page_map(0, (void*)(fault_addr & ~0x3FF), phys, 0);

        return -E_SUCCESS;

        problem:
        panic("Writing page faults currently remain unhandled");
        return -E_GENERIC;
}

int vm_user_fault_read(addr_t fault_addr, int mapped)
{
        /**
         * \todo Add permission checking
         * \todo Add correct handling
         */
        panic("User space page faults currently remain unhandled");
        return -E_NOFUNCTION;
}

int vm_kernel_fault_read(addr_t fault_addr, int mapped, addr_t eip)
{
        if (!mapped) {
                /**
                 * \todo Reload pages here when swapping is written
                 */
                printf(
                                "The kernel wants to read garbage from invalid memory.\n");
                printf("Address:   %X\n", (int)fault_addr);
                printf("Fault eip: %X\n", (int)eip);
                panic("Reason enough to panic, I'd say!");
        }

        /**
         * \todo Add permission checking
         * \todo Add propper handling
         */

        printf("Faulting on %X\n", fault_addr);

        panic("Reading page faults currently remain unhandled");
        return -E_NOFUNCTION;
}
#pragma GCC diagnostic pop

/**
 * @}
 * \file
 */
