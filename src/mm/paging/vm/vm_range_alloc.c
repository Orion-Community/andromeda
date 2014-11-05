/* Andromeda
 * Copyright (C) 2014  Bart Kuivenhoven
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

#include <mm/vm.h>
#include <thread.h>
#include <andromeda/system.h>

#define MIN_CACHED_DESCRIPTORS 0x3
#define MAX_CACHED_DESCRIPTORS 0x40
#define MIN_STATIC_DESCRIPTORS 0x3
#define MAX_STATIC_DESCRIPTORS 0x10

static struct vm_range_buffer vm_buffer;
static struct vm_range_buffer vm_static;

static struct vm_range_descriptor static_ranges[0x40];

static uint32_t vm_range_alloc_ready = 0;
static mutex_t vm_dynamic_initialised = mutex_unlocked;
static uint32_t vm_dynamic_range_ready = 0;
int mm_vm_range_buffer_start = 0;

#ifdef SLAB
static struct mm_cache* vm_range_cache = NULL;
#endif

/*
 * The goal of this special allocator is to make sure that when the memory
 * allocator comes asking for memory, there is NEVER a problem the allocator
 * being asked to subsequently be asked to provide memory for a descriptor,
 * in order to meet the request.
 *
 */

/**
 * \fn vm_range_alloc
 * \return New range descriptor
 *
 * Because of the places this is called, the virtual memory allocation system
 * can be assumed to be in a locked state. As such, any allocations to refill
 * the allocation buffer will fail if they require more memory from the heap.
 */
static struct vm_range_descriptor*
vm_range_alloc_ptr(struct vm_range_buffer* buffer)
{
        /*
         * This lock can block, as it should only happen rarely, and only for
         * range descriptor allocations.
         * The lock shouldn't last long anyway.
         *
         * Also, yes, I know that this doesn't scale too well. If you have a fix
         * please feel free to implement.
         */
        mutex_lock(&buffer->get_lock);

        struct vm_range_descriptor* desc = NULL;
        int length = -E_LOCKED;

        /* If not enough resources are available, return error */
        while (length == -E_LOCKED)
                length = semaphore_try_dec(&buffer->length);

        if (length == -E_OUT_OF_RESOURCES) {
                goto cleanup;
        }

        /*
         * Since we managed to decrease the number of resources, we can now
         * take one.
         */
        desc = buffer->head;
        buffer->head = desc->next;
        buffer->head->prev = NULL;

        /* Clean up the allocated resource */
        memset(desc, 0, sizeof(*desc));

        /* Clear the lock and return blank or the allocated resource */
cleanup:
        mutex_unlock(&buffer->get_lock);
        return desc;
}

/**
 * \fn vm_range_alloc
 * \return The newly allocated range descriptor.
 *
 * This allocates from the statically allocated pool, if still available,
 * otherwise it will switch to the allocator based pool of descriptors.
 */
struct vm_range_descriptor* vm_range_alloc()
{
        struct vm_range_descriptor* desc = vm_range_alloc_ptr(&vm_static);
        if (desc != NULL) {
                desc->static_alloc = 1;
        } else if (vm_dynamic_range_ready != 0) {
                desc = vm_range_alloc_ptr(&vm_buffer);
        }
        return desc;
}

/**
 * \fn vm_range_free_dynamic
 * \brief Free up a range descriptor
 * \param descriptor
 * \return A generic error code
 */
static int
vm_range_reset(descriptor, buffer)
struct vm_range_descriptor* descriptor;
struct vm_range_buffer* buffer;
{
        int error = -E_SUCCESS;
        if (descriptor == NULL || buffer == NULL)
                return -E_NULL_PTR;

        if (vm_range_alloc_ready == 0)
                return -E_NOT_YET_INITIALISED;

        int locked = mutex_test(&buffer->put_lock);
        if (locked == mutex_locked)
                return -E_LOCKED;

        int length = -E_LOCKED;
        do {
                length = semaphore_try_inc(&buffer->length);
        } while (length == -E_LOCKED);

        if (length < 0) {
                error = -E_OUT_OF_RESOURCES;
                goto cleanup;
        }
        buffer->tail->next = descriptor;
        descriptor->prev = buffer->tail;
        buffer->tail = descriptor;

cleanup:
        mutex_unlock(&buffer->put_lock);
        return error;
}

/**
 * \fn vm_range_free
 * \brief Used to free up vm range descriptors
 * \param descriptor
 * \return A generic error code
 */
int vm_range_free(struct vm_range_descriptor* descriptor)
{
        /* Check precondition */
        if (descriptor == NULL)
                return -E_NULL_PTR;

        /* Clear the descriptor */
        int32_t static_alloc = descriptor->static_alloc;
        memset (descriptor, 0, sizeof(*descriptor));

        /* If allocated from the static pool, return to the static pool */
        int error = -E_SUCCESS;
        if (static_alloc != 0) {
                descriptor->static_alloc = 1;
                error = vm_range_reset(descriptor, &vm_static);
        } else if (static_alloc == 0) {
                /* Has the dynamic system been implemented yet? (it should be)*/
                if (vm_dynamic_range_ready == 0)
                        return -E_NOT_YET_INITIALISED;

                /* Try to add this node to the list again. */
                error = vm_range_reset(descriptor, &vm_buffer);
        }

        /* If we've been able to push this to the list, return success */
        if (error == -E_SUCCESS)
                return error;
        descriptor->static_alloc = 0;

        /*
         * We haven't been able to add this to the buffer, so let's free up
         * some space!
         */
#ifdef SLAB
        mm_cache_free(vm_range_cache, descriptor);
#else
        kfree(descriptor);
#endif
        /* Time to return success */
        return -E_SUCCESS;
}

/**
 * \fn vm_range_update_dynamic
 * \brief Updates the range descriptor tables.
 * This function should be called by general purpose memory allocators as soon
 * as they have memory available, in order to meet the requirements of the
 * virtual memory system.
 */
static int vm_range_update_dynamic()
{
        /*
         * If the queue is full, return
         */
        int64_t length = semaphore_try_get(&vm_buffer.length);
        if (length == -1)
                return -E_SUCCESS;

        /*
         * Get the lock, or if someone is already working on this, return success
         */
        int lock = mutex_test(&vm_buffer.put_lock);
        if (lock == mutex_locked)
                return -E_SUCCESS;

        struct vm_range_descriptor* desc = NULL;

        do {
                length = semaphore_try_inc(&vm_buffer.length);
                /* If queue is full, we're done */
                if (length == -E_OUT_OF_RESOURCES)
                        break;
                /* If counter is locked, try again */
                if (length == -E_LOCKED)
                        continue;

                /* Allocate the new node */
#ifdef SLAB
                desc = mm_cache_alloc(vm_range_cache, CACHE_ALLOC_NO_UPDATE);
#else
                desc = kmalloc(sizeof(*desc));
#endif
                /* If allocations fail, come back another time */
                if (desc == NULL)
                        break;

                /* Prepare the node for insertion */
                memset(desc, 0, sizeof(*desc));

                /* Insert the node */
                vm_buffer.tail->next = desc;
                desc->next = vm_buffer.tail;
                vm_buffer.tail = desc;

        } while (length < MAX_CACHED_DESCRIPTORS);

        /* And we're done */
        mutex_unlock(&vm_buffer.put_lock);

        return -E_SUCCESS;
}


static int vm_range_alloc_dynamic_init()
{
        vm_range_alloc_ready = 0;
        /* Because this system is run later in boot, make this check atomic */
        int initialised = mutex_test(&vm_dynamic_initialised);
        if (initialised == mutex_locked) {
                return -E_ALREADY_INITIALISED;
        }
        /*
         * Prepare the memory allocator if necessary.
         */
#ifdef SLAB
        vm_range_cache = mm_cache_init(
                        "vm_range_cache",
                        sizeof(struct vm_range_descriptor),
                        1, NULL, NULL);
        if (vm_range_cache == NULL) {
                mutex_unlock(&vm_dynamic_initialised);
                return -E_NOT_YET_INITIALISED;
        }

#endif
        /* Initialise the buffer structure */
        memset(&vm_buffer, 0, sizeof(vm_buffer));

        idx_t i = 0;
        /* Prepare the semaphore */
        semaphore_init(&vm_buffer.length, 0, MIN_CACHED_DESCRIPTORS, MAX_CACHED_DESCRIPTORS);

        /* Load the buffer with descriptors */
        for (; i < MAX_CACHED_DESCRIPTORS; i++) {
                struct vm_range_descriptor* desc;
                /* Allocate the descriptor */
#ifdef SLAB
                desc = mm_cache_alloc(vm_range_cache, CACHE_ALLOC_NO_UPDATE);
#else
                desc = kmalloc(sizeof(*desc));
#endif
                /*
                 * If out of memory already, we can't guarantee proper system
                 * operation. We might as well panic now.
                 */
                if (desc == NULL)
                        break;

                /* Clear the descriptor of all garbage */
                memset(desc, 0 , sizeof(*desc));

                /* Place the descriptor in the list. */
                if (i == 0) {
                        vm_buffer.head = desc;
                        vm_buffer.tail = desc;
                } else {
                        vm_buffer.tail->next = desc;
                        desc->prev = vm_buffer.tail;
                        vm_buffer.tail = desc;
                }
                /* Increment the list size by one */
                semaphore_try_inc(&vm_buffer.length);
        }
        /* Mark the dynamic system ready for use */
        vm_dynamic_range_ready = 1;

        return -E_SUCCESS;
}

/**
 * \fn vm_range_update
 * \return Error code
 *
 * This function is called by memory allocators to make sure that the buffers
 * stay topped up.
 */
int vm_range_update()
{
        /*
         * Make sure that the dynamic allocator has been initialised.
         */
        if (vm_dynamic_range_ready == 0) {
                if (mm_vm_range_buffer_start == 0) {
                        return -E_SUCCESS;
                }
                vm_range_alloc_dynamic_init();
                /*
                 * If the allocator has just been initialised, all the nodes
                 * should be in place. Don't worry about updating and just
                 * return.
                 */
                return -E_SUCCESS;
        }

        /*
         * Make sure to actually allocate the descriptors if missing.
         */
        return vm_range_update_dynamic();
}

int vm_range_alloc_init()
{
        /* If already initialised, return */
        if (vm_range_alloc_ready != 0)
                return -E_ALREADY_INITIALISED;

        /*
         * Make sure this code doesn't run anymore
         * Because this code is called on first boot, and no other tasks/cpu's
         * should be running at this moment in time, there's no need to make
         * this blockade atomic
         */
        vm_range_alloc_ready = 1;

        /* Initialise the static pool header */
        memset(&vm_static, 0, sizeof(vm_static));
        semaphore_init(&vm_static.length, 0, MIN_STATIC_DESCRIPTORS, MAX_CACHED_DESCRIPTORS);

        /* Add the nodes to the static pool. */
        idx_t i = 0;
        /* memset the nodes to 0 */
        memset (&static_ranges, 0, sizeof(static_ranges));

        for (; i < MAX_STATIC_DESCRIPTORS; i++) {

                /* Add the nodes to the queue */
                switch (i) {
                case 0:
                        /* In case of head */
                        vm_static.head = &static_ranges[i];
                        vm_static.tail = &static_ranges[i];
                        break;
                default:
                        /* And all other nodes */
                        vm_static.tail->next = &static_ranges[i];
                        static_ranges[i].prev = vm_static.tail;
                        vm_static.tail = &static_ranges[i];
                        break;
                }

                /* Make sure to increase the list length counter */
                semaphore_inc(&vm_static.length);
        }

        return -E_SUCCESS;
}
